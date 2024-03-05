#pragma once
#include "commons.h"

template<typename T>
class smart_pool
{
    T* m_pool;
    size_t m_size;

    explicit smart_pool(T* pool, size_t size = sizeof(T)) : m_pool(pool), m_size(size) {}

public:

    template<typename U, POOL_TYPE pool_type = NonPagedPool>
    friend smart_pool<U, pool_type> make_smart(size_t elements);

    T* get() { return m_pool; }

    operator bool() { return m_pool != nullptr; }

    T* operator->() { return m_pool; }

    T& operator*() { return *m_pool; }

    void& operator[] (size_t index) = delete;

    T& operator[] (size_t index)
    {
        return m_pool[index];
    }

    void& at(size_t index) = delete;

    T& at(size_t index) 
    {
        if (index >= m_size / sizeof(T))
        {
            return T{}; // DEFAULT CONSTRUCTOR OR ZERO INIT
		}
		return m_pool[index];
    }

    smart_pool(const smart_pool&) = delete;
    smart_pool& operator=(const smart_pool&) = delete;

    smart_pool(smart_pool&& other) noexcept : m_pool(other.m_pool) { other.m_pool = nullptr; }
    smart_pool& operator=(smart_pool&& other) noexcept
    {
        if (this != &other)
        {
            m_pool = other.m_pool;
            other.m_pool = nullptr;

            m_size = other.m_size;
        }
        return *this;
    }

    ~smart_pool() {
        if (!m_pool) return;
        
        for(size_t i = 0; i < m_size / sizeof(T); i++)
        {
			m_pool[i].~T();
		}

        ExFreePoolWithTag(m_pool, 0);
    }
};

template<typename U, POOL_TYPE pool_type = NonPagedPool>
inline smart_pool<U> make_smart(size_t elements = 1)
{
    return smart_pool<T, pool_type>(static_cast<T*>(ExAllocatePoolWithTag(pool_type, sizeof(T) * elements, 0)), sizeof(T) * elements);
}