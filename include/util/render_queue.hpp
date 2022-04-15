#include <condition_variable>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>

#include "util.hpp"

using u32 = std::uint_fast32_t;

template <typename T> requires
requires(u32 h, T fn) { { fn(h) } -> std::same_as<void>; }
struct render_queue
{
protected:
    const u32 height;
    const u32 count;
    const T fn;
    bool running = true;
    std::atomic<u32> current_height = 0;

    using worker = std::jthread;

    std::condition_variable cv;
    std::unique_ptr<worker[]> queue;

protected:
    void work()
    {
        while(running)
        {
            u32 h;
            if(get(h))
            {
                fn(h); // render by line
            }
            else
            {
                cv.notify_one();
                std::this_thread::yield();
            }
        }
    }

    void stop() { running = false; }

    bool get(u32& i)
    {
        if(current_height < height)
        {
            i = current_height;
            current_height += 1;
            return true;
        }
        return false;
    }

    template <typename U>
    void handle_threads(U fn)
    {
        for(auto i : range(count))
        {
            fn(queue[i]);
        }
    }

public:
    render_queue() = delete;
    render_queue(const render_queue&) = delete;
    render_queue(render_queue&&) = delete;
    render_queue& operator = (const render_queue&) = delete;
    render_queue& operator = (render_queue&&) = delete;

    render_queue(u32 thread_count, u32 height, T fn)
        : height(height)
        , count(thread_count)
        , fn(fn)
        , queue(std::make_unique<worker[]>(count))
    {
        handle_threads([&](worker& w)
        {
            w = worker{&render_queue::work, this};
        });
    }

    ~render_queue()
    {
        if(running)
        {
            wait();
        }
    }

    void wait()
    {
        std::mutex m;
        std::unique_lock lock(m);
        cv.wait(lock);
        // cv.wait(lock, [&](){ return current_height >= height; });
        stop();
        handle_threads([&](worker& w)
        {
            if(w.joinable())
                w.join();
        });
    }
};
