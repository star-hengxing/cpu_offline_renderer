#pragma once

#include <cstdint>
#include <utility>

// thread not safe, without week count
template <typename T>
struct Rc
{
private:
    struct rc_box
    {
        T value;
        std::size_t strong;

        rc_box() = delete;
        rc_box(const rc_box&) = delete;
        rc_box& operator = (const rc_box& rc) = delete;
        rc_box& operator = (rc_box&& rc) = delete;

        template <typename... Args>
        rc_box(Args&&... args) noexcept
            : value(std::forward<Args>(args)...), strong(1) {}

        void inc_strong()
        {
            strong += 1;
        }

        void dec_strong()
        {
            strong -= 1;
        }

        bool strong_is_zero()
        {
            return strong == 0;
        }
    };

    rc_box* ptr;

private:
    Rc(rc_box* rc) : ptr(rc) {}

public:
    constexpr Rc() : ptr(nullptr) {}

    Rc(const Rc<T>& rc) : ptr(rc.ptr)
    {
        ptr->inc_strong();
    }

    Rc(Rc<T>&& rc) noexcept : ptr(rc.ptr)
    {
        rc.ptr = nullptr;
    }

    Rc<T>& operator = (const Rc<T>& rc)
    {
        ptr = rc.ptr;
        ptr->inc_strong();
        return *this;
    }

    Rc<T>& operator = (Rc<T>&& rc) noexcept
    {
        ptr = rc.ptr;
        rc = nullptr;
        return *this;        
    }
    
    ~Rc()
    {
        if(ptr == nullptr) return;

        ptr->dec_strong();
        if(!ptr->strong_is_zero()) return;

        delete ptr;
        ptr = nullptr;
    }

    template <typename... Args>
    static auto make(Args&&... args) -> Rc<T>
    {
        return Rc<T>(new rc_box(std::forward<Args>(args)...));
    }

    T* get() const noexcept
    {
        return &(ptr->value);
    }

    T operator * () const noexcept
    {
        return *get();
    }

    T* operator -> () const noexcept
    {
        return get();
    }

    operator bool () const
    {
        return ptr != nullptr;
    }
};
