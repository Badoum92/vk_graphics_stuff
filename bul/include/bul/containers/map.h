#pragma once

#include <utility>
#include <type_traits>

#include "bul/bul.h"
#include "bul/hash.h"
#include "bul/containers/vector.h"

namespace bul
{
template <typename Key, typename Val>
class map
{
public:
    using this_t = map<Key, Val>;

    explicit map(size_t capacity_ = 2)
    {
        // capacity_ = align_pow2(capacity_, 2);
        slots_.resize(capacity_);
        keys_.reserve(capacity_);
        values_.reserve(capacity_);
    }

    template <typename K, typename... Args>
    Val* emplace(K&& key_, Args&&... args)
    {
        if (should_grow())
        {
            grow();
        }

        Key key = std::forward<K>(key_);
        size_t cap = capacity();
        uint32_t hash = bul::hash(key);
        slot slot{hash & 0xff, 0, 1, uint32_t(values_.size())};
        const Key& new_key = keys_.emplace_back(std::move(key));
        values_.emplace_back(std::forward<Args>(args)...);

        for (size_t i_slot = mod_pow2(hash, cap);; i_slot = mod_pow2(i_slot + 1, cap))
        {
            this_t::slot& cur_slot = slots_[i_slot];

            if (!cur_slot.full)
            {
                std::swap(cur_slot, slot);
                return &values_.back();
            }

            if (cur_slot.hash == slot.hash && keys_[cur_slot.index] == new_key)
            {
                keys_.pop_back();
                values_.pop_back();
                return nullptr;
            }

            if (slot.psl > cur_slot.psl)
            {
                std::swap(cur_slot, slot);
            }

            slot.psl += 1;
        }

        return nullptr;
    }

    bool erase(const Key& key_)
    {
        slot* found_slot = find_slot(key_);
        if (found_slot == nullptr)
        {
            return false;
        }

        Val& value = values_[found_slot->index];
        Key& key = keys_[found_slot->index];
        uint32_t last_index = values_.size() - 1;

        if (found_slot->index != last_index)
        {
            key = std::move(keys_.back());
            value = std::move(values_.back());
        }

        size_t cap = capacity();
        uint32_t hash = bul::hash(key);
        size_t i_slot = mod_pow2(hash, cap);

        for (;; i_slot = mod_pow2(i_slot + 1, cap))
        {
            slot& slot = slots_[i_slot];
            if (slot.index == last_index)
            {
                slot.index = found_slot->index;
                break;
            }
        }

        keys_.pop_back();
        values_.pop_back();

        found_slot->full = 0;
        found_slot->hash = 0;
        found_slot->index = 0;
        found_slot->psl = 0;

        for (size_t i_slot = found_slot - slots_.data();;)
        {
            size_t i_next_slot = mod_pow2(i_slot + 1, cap);
            this_t::slot& slot = slots_[i_slot];
            this_t::slot& next_slot = slots_[i_next_slot];

            if (!next_slot.full || next_slot.psl == 0)
            {
                break;
            }

            std::swap(slot, next_slot);
            slot.psl -= 1;
            i_slot = i_next_slot;
        }

        return true;
    }

    Val* operator[](const Key& key)
    {
        slot* slot = find_slot(key);
        if (slot != nullptr)
        {
            return &values_[slot->index];
        }
        return nullptr;
    }

    const Val* operator[](const Key& key) const
    {
        slot* slot = const_cast<map<Key, Val>*>(this)->find_slot(key);
        if (slot != nullptr)
        {
            return &values_[slot->index];
        }
        return nullptr;
    }

    size_t size() const
    {
        return values_.size();
    }

    size_t capacity() const
    {
        return slots_.size();
    }

    struct entry
    {
        const Key* key = nullptr;
        Val* val = nullptr;
    };

    template <typename T>
    struct map_iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using this_t = map_iterator<T>;

        map_iterator(uint32_t index, uint32_t size, const Key* keys, Val* values)
            : _index(index)
            , _size(size)
            , _keys(keys)
            , _values(values)
            , _entry(keys, values)
        {}

        reference operator*()
        {
            return _entry;
        }

        pointer operator->()
        {
            return &_entry;
        }

        this_t& operator++()
        {
            if (_index >= _size)
            {
                return *this;
            }
            ++_index;
            _entry.key = &_keys[_index];
            _entry.val = &_values[_index];
            return *this;
        }

        this_t operator++(int)
        {
            this_t tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const this_t& a, const this_t& b)
        {
            return a._index == b._index && a._keys == b._keys;
        }

        friend bool operator!=(const this_t& a, const this_t& b)
        {
            return !(a == b);
        }

        uint32_t _index = 0;
        uint32_t _size = 0;
        const Key* _keys = nullptr;
        Val* _values = nullptr;
        entry _entry;
    };

    using iterator = map_iterator<entry>;
    using const_iterator = map_iterator<const entry>;

    const_iterator begin() const
    {
        return iterator{0, uint32_t(keys_.size()), keys_.data(), values_.data()};
    }

    iterator begin()
    {
        return iterator{0, uint32_t(keys_.size()), keys_.data(), values_.data()};
    }

    const_iterator end() const
    {
        return iterator{uint32_t(keys_.size()), uint32_t(keys_.size()), keys_.data(), values_.data()};
    }

    iterator end()
    {
        return iterator{uint32_t(keys_.size()), uint32_t(keys_.size()), keys_.data(), values_.data()};
    }

private:
    struct slot
    {
        uint64_t hash : 8;
        uint64_t psl : 24;
        uint64_t full : 1;
        uint64_t index : 31;
    };
    static_assert(sizeof(slot) == sizeof(uint64_t));

    static size_t mod_pow2(size_t x, size_t y)
    {
        return x & (y - 1);
    }

    slot* find_slot(const Key& key)
    {
        size_t cap = capacity();
        uint32_t hash = bul::hash(key);
        slot slot{hash & 0xff, 0, 1, 0};

        for (size_t i_slot = mod_pow2(hash, cap);; i_slot = mod_pow2(i_slot + 1, cap))
        {
            this_t::slot& cur_slot = slots_[i_slot];

            if (!cur_slot.full || slot.psl > cur_slot.psl)
            {
                return nullptr;
            }

            if (cur_slot.hash == slot.hash && keys_[cur_slot.index] == key)
            {
                return &cur_slot;
            }

            slot.psl += 1;
        }
    }

    bool should_grow()
    {
        return values_.size() >= capacity() * load_factor;
    }

    void grow()
    {
        slots_.resize(capacity() * 2);
        memset(slots_.data(), 0, sizeof(slot) * slots_.size());

        size_t cap = capacity();
        for (uint32_t i_key = 0; i_key < keys_.size(); ++i_key)
        {
            uint32_t hash = bul::hash(keys_[i_key]);
            slot slot{hash & 0xff, 0, 1, i_key};

            for (size_t i_slot = mod_pow2(hash, cap);; i_slot = mod_pow2(i_slot + 1, cap))
            {
                this_t::slot& cur_slot = slots_[i_slot];

                if (!cur_slot.full)
                {
                    std::swap(cur_slot, slot);
                    break;
                }

                if (slot.psl > cur_slot.psl)
                {
                    std::swap(cur_slot, slot);
                    continue;
                }

                slot.psl += 1;
            }
        }
    }

    vector<slot> slots_;
    vector<Key> keys_;
    vector<Val> values_;

    static inline constexpr float load_factor = 0.75f;
};
} // namespace bul
