#pragma once

#include <type_traits>
#include <iterator>

namespace bul
{
template <typename T, size_t CAPACITY>
class StaticQueue
{
public:
    constexpr StaticQueue() = default;

    constexpr ~StaticQueue()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = begin_; i != end_; i = (i + 1) % CAPACITY)
            {
                data_[i].~T();
            }
        }
    }

    constexpr StaticQueue(const StaticQueue&) = default;
    constexpr StaticQueue& operator=(const StaticQueue&) = default;
    constexpr StaticQueue(StaticQueue&&) = default;
    constexpr StaticQueue& operator=(StaticQueue&&) = default;

    constexpr bool push_back(const T& val)
    {
        return emplace_back(val);
    }

    constexpr bool push_back(T&& val)
    {
        return emplace_back(std::move(val));
    }

    template <typename... Args>
    constexpr bool emplace_back(Args&&... args)
    {
        if (size_ == CAPACITY)
        {
            return false;
        }
        new (data_ + end_) T(std::forward<Args>(args)...);
        end_ = (end_ + 1) % CAPACITY;
        ++size_;
        return true;
    }

    constexpr bool pop_front()
    {
        if (size_ == 0)
        {
            return false;
        }
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            data_[begin_].~T();
        }
        begin_ = (begin_ + 1) % CAPACITY;
        --size_;
        return true;
    }

    constexpr size_t capacity() const
    {
        return CAPACITY;
    }

    constexpr size_t size() const
    {
        return size_;
    }

    constexpr bool empty() const
    {
        return size() == 0;
    }

    constexpr T* begin()
    {
        return data_;
    }

    constexpr T* end()
    {
        return data_ + size_;
    }

    constexpr const T& front() const
    {
        return data_[begin_];
    }

    constexpr T& front()
    {
        return data_[begin_];
    }

    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator(size_t index, pointer base)
            : index_(index)
            , base_(base)
        {}

        reference operator*() const
        {
            return base_[index_];
        }

        pointer operator->()
        {
            return base_ + index_;
        }

        Iterator& operator++()
        {
            index_ = (index_ + 1) % CAPACITY;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& a, const Iterator& b)
        {
            return a.base_ == b.base_ && a.index_ == b.index_;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b)
        {
            return a.base_ != b.base_ || a.index_ != b.index_;
        }

    private:
        size_t index_;
        pointer base_;
    };

    Iterator begin() const
    {
        return Iterator(begin_, data_);
    }

    Iterator end() const
    {
        return Iterator(end_, data_);
    }

private:
    size_t begin_ = 0;
    size_t end_ = 0;
    size_t size_ = 0;
    alignas(T) T data_[CAPACITY] = {};
};
} // namespace bul
