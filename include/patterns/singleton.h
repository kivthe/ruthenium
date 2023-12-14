#pragma once

#include <type_traits>
#include <utility>

namespace ruthen
{

namespace patterns
{

template<typename T>
class Singleton
{
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    template<typename... Args>
    static T* StaticInstance(Args&&... args)
    {
        static_assert(std::is_constructible<T, Args...>::value, "Invalid argument list for type construction");
        static T instance(std::forward<Args>(args)...);
        return &instance;
    }
    template<typename... Args>
    static T* DynamicInstance(Args&&... args)
    {
        static_assert(std::is_constructible<T, Args...>::value, "Invalid argument list for type construction");
        T* ptr = new(std::nothrow) T(std::forward<Args>(args)...);
        return ptr;
    }

protected:
    Singleton() {};

};

}

}