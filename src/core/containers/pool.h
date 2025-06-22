#pragma once

#include <stdint.h>
#include <stdlib.h>

#define _max(a, b)      ((a) > (b) ? (a) : (b))
#define FREE_FLAG       (1ull << 63)
#define SET_FREE(PTR)   ((T*)(((uint64_t)(PTR)) | FREE_FLAG))
#define RESET_FREE(PTR) ((T*)(((uint64_t)(PTR)) & ~FREE_FLAG))

struct pool_node
{
    pool_node* next;
    // uint8_t data[0];
};

template <typename T>
struct pool
{
    T* free_list;
    pool_node* first_node;
    uint32_t size;
};

constexpr uint32_t pool_elements_per_node = 64;

template <typename T>
T* _pool_at(pool_node* node, uint32_t index)
{
    return (T*)((uint8_t*)(node + 1) + index * _max(sizeof(T), sizeof(void*)));
}

template <typename T>
void _pool_add_node(pool<T>* pool)
{
    pool_node* node = (pool_node*)malloc(sizeof(node) + pool_elements_per_node * _max(sizeof(T), sizeof(void*)));
    node->next = nullptr;
    for (uint32_t i = 0; i < pool_elements_per_node; ++i)
    {
        *(T**)_pool_at<T>(node, i) = _pool_at<T>(node, i + 1);
    }
    *(T**)_pool_at<T>(node, pool_elements_per_node - 1) = nullptr;
    if (pool->first_node)
    {
        pool->first_node->next = node;
    }
    pool->first_node = node;
    pool->free_list = (T*)(node + 1);
    pool->free_list = SET_FREE(pool->free_list);
}

template <typename T>
pool<T> pool_create()
{
    pool<T> pool = {};
    _pool_add_node(&pool);
    pool.size = 0;
    return pool;
}

template <typename T>
void pool_destroy(pool<T>* pool)
{
    for (pool_node* node = pool->first_node; node != nullptr;)
    {
        pool_node* next = node->next;
        free(node);
        node = next;
    }
    pool->free_list = nullptr;
    pool->first_node = nullptr;
    pool->size = 0;
}

template <typename T>
T* pool_alloc(pool<T>* pool)
{
    static_assert(sizeof(T) >= sizeof(void*));
    if (pool->free_list == nullptr)
    {
        _pool_add_node(pool);
    }
    T* ptr = RESET_FREE(pool->free_list);
    pool->free_list = *(T**)ptr;
    pool->size++;
    return ptr;
}

template <typename T>
void pool_free(pool<T>* pool, T* ptr)
{
    *(T**)ptr = pool->free_list;
    pool->free_list = SET_FREE(ptr);
    pool->size--;
}

template <typename T>
T* pool_get_data(pool_node* node)
{
    return (T*)node + 1;
}

inline bool pool_is_element_free(void* element)
{
    return ((uint64_t)element) & FREE_FLAG;
}

#undef _max
#undef FREE_FLAG
#undef SET_FREE
#undef RESET_FREE