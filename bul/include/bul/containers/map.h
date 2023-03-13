#pragma once

#include "bul/bul.h"
#include "bul/hash.h"

#include <utility>
#include <concepts>
#include <vector>
#include <bit>

namespace bul
{
template <typename KEY_T, typename VAL_T>
requires std::equality_comparable<KEY_T>
class Map
{
public:
    struct Entry
    {
        template <typename... Args>
        Entry(const KEY_T& k, Args&&... args)
            : key{k}
            , val{std::forward<Args>(args)...}
        {}

        template <typename... Args>
        Entry(KEY_T&& k, Args&&... args)
            : key{std::move(k)}
            , val{std::forward<Args>(args)...}
        {}

        KEY_T key;
        VAL_T val;
    };

    explicit Map(size_t capacity_ = 4)
    {
        ASSERT(std::has_single_bit(capacity_));
        slots_.resize(capacity_);
        entries_.reserve(capacity_);
    }

    template <typename K, typename V>
    requires std::constructible_from<KEY_T, K> && std::constructible_from<VAL_T, V>
    Entry* insert(K&& key, V&& val)
    {
        return emplace(std::forward<K>(key), std::forward<V>(val));
    }

    template <typename K, typename... Args>
    requires std::constructible_from<KEY_T, K> && std::constructible_from<VAL_T, Args...>
    Entry* emplace(K&& key, Args&&... args)
    {
        if (should_grow())
        {
            grow();
        }

        size_t cap = capacity();
        uint32_t hash = bul::hash(key);
        Slot slot{hash & 0xff, 0, 1, uint32_t(entries_.size())};
        Entry& entry = entries_.emplace_back(std::forward<K>(key), std::forward<Args>(args)...);

        for (size_t i_slot = mod_pow2(hash, cap);; i_slot = mod_pow2(i_slot + 1, cap))
        {
            Slot& cur_slot = slots_[i_slot];
            Entry& cur_entry = entries_[cur_slot.index];

            if (!cur_slot.full)
            {
                std::swap(cur_slot, slot);
                return &entries_.back();
            }

            if (cur_slot.hash == slot.hash && cur_entry.key == entry.key)
            {
                entries_.pop_back();
                return nullptr;
            }

            if (slot.psl > cur_slot.psl)
            {
                std::swap(cur_slot, slot);
                continue;
            }

            slot.psl += 1;
        }

        return nullptr;
    }

    bool erase(const KEY_T& key)
    {
        Slot* found_slot = find(key);
        if (found_slot == nullptr)
        {
            return false;
        }

        Entry& entry = entries_[found_slot->index];
        uint32_t last_index = entries_.size() - 1;

        if (found_slot->index != last_index)
        {
            entry = std::move(entries_.back());
        }

        size_t cap = capacity();
        uint32_t hash = bul::hash(entry.key);
        size_t i_slot = mod_pow2(hash, cap);

        for (;; i_slot = mod_pow2(i_slot + 1, cap))
        {
            Slot& slot = slots_[i_slot];
            if (slot.index == last_index)
            {
                slot.index = found_slot->index;
                break;
            }
        }

        entries_.pop_back();

        found_slot->full = 0;
        found_slot->hash = 0;
        found_slot->index = 0;
        found_slot->psl = 0;

        for (size_t i_slot = found_slot - slots_.data();;)
        {
            size_t i_next_slot = mod_pow2(i_slot + 1, cap);
            Slot& slot = slots_[i_slot];
            Slot& next_slot = slots_[i_next_slot];

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

    Entry* operator[](const KEY_T& key)
    {
        Slot* slot = find(key);
        if (slot != nullptr)
        {
            return &entries_[slot->index];
        }
        return nullptr;
    }

    const Entry* operator[](const KEY_T& key) const
    {
        return const_cast<Map<KEY_T, VAL_T>*>(this)->find_(key);
    }

    size_t size() const
    {
        return entries_.size();
    }

    size_t capacity() const
    {
        return slots_.size();
    }

    const auto begin() const
    {
        return entries_.begin();
    }

    auto begin()
    {
        return entries_.begin();
    }

    const auto end() const
    {
        return entries_.end();
    }

    auto end()
    {
        return entries_.end();
    }

private:
    struct Slot
    {
        uint32_t hash : 8;
        uint32_t psl : 24;
        uint32_t full : 1;
        uint32_t index : 31;
    };
    static_assert(sizeof(Slot) == sizeof(uint64_t));

    static size_t mod_pow2(size_t x, size_t y)
    {
        return x & (y - 1);
    }

    Slot* find(const KEY_T& key)
    {
        size_t cap = capacity();
        uint32_t hash = bul::hash(key);
        Slot slot{hash & 0xff, 0, 1, 0};

        for (size_t i_slot = mod_pow2(hash, cap);; i_slot = mod_pow2(i_slot + 1, cap))
        {
            Slot& cur_slot = slots_[i_slot];
            Entry& cur_entry = entries_[cur_slot.index];

            if (!cur_slot.full || slot.psl > cur_slot.psl)
            {
                return nullptr;
            }

            if (cur_slot.hash == slot.hash && cur_entry.key == key)
            {
                return &cur_slot;
            }

            slot.psl += 1;
        }
    }

    bool should_grow()
    {
        return entries_.size() >= capacity() * load_factor;
    }

    void grow()
    {
        slots_.resize(capacity() * 2);
        memset(slots_.data(), 0, sizeof(Slot) * slots_.size());

        size_t cap = capacity();
        for (size_t i_entry = 0; i_entry < entries_.size(); ++i_entry)
        {
            const Entry& entry = entries_[i_entry];
            uint32_t hash = bul::hash(entry.key);
            Slot slot{hash & 0xff, 0, 1, uint32_t(i_entry)};

            for (size_t i_slot = mod_pow2(hash, cap);; i_slot = mod_pow2(i_slot + 1, cap))
            {
                Slot& cur_slot = slots_[i_slot];

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

    std::vector<Slot> slots_;
    std::vector<Entry> entries_;

    static inline constexpr float load_factor = 0.75;
};
} // namespace bul
