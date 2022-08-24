#pragma once

#include <cstdint>
#include <utility>

template <typename T>
struct Box
{
private:
    T* ptr;

public:
    constexpr Box(T* ptr = nullptr) : ptr(ptr) {}

    Box(const Box<T>&) = delete;
    Box<T>& operator = (const Box<T>&) = delete;

    Box(Box<T>&& box) noexcept : ptr(box.ptr)
    {
        box.ptr = nullptr;
    }

    Box<T>& operator = (Box<T>&& box) noexcept
    {
        ptr = box.ptr;
        box.ptr = nullptr;
        return *this;        
    }
    
    ~Box()
    {
        if(ptr != nullptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }

    template <typename... Args>
    static Box<T> make(Args&&... args)
    {
        return {new T(std::forward<Args>(args)...)};
    }

    T* get() const noexcept
    {
        return ptr;
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
