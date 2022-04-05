#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
// #include <array>

#include "util.hpp"

template <typename T> requires
requires(usize h, T fn) { { fn(h) } -> std::same_as<void>; }
struct render_queue
{
protected:
    const usize height;
    const T fn;
    bool running = true;
    std::atomic<usize> current_height = 0;

    using worker = std::jthread;

    std::condition_variable cv;
    std::vector<worker> queue;

protected:
    void work()
    {
        while(running)
        {
            usize h;
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

    void start() { running = true; }
    void stop() { running = false; }

    bool get(usize& i)
    {
        if(current_height < height)
        {
            i = current_height;
            current_height += 1;
            return true;
        }
        return false;
    }

public:
    render_queue(usize height, T fn) : height(height), fn(fn)
    {
        const auto size = worker::hardware_concurrency();
        queue.reserve(size);
        for_each(size, [&]()
        { 
            queue.emplace_back(worker{&render_queue::work, this});
        });
    }

    ~render_queue()
    {
        stop();
    }

    void wait()
    {
        std::mutex m;
        std::unique_lock lock(m);
        cv.wait(lock);
        // cv.wait(lock, [&](){ return current_height >= height; });
    }

    void run_and_wait()
    {
        start();
        wait();
        stop();
    }
};
