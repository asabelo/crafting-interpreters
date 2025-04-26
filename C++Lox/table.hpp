
#pragma once

#include "common.hpp"
#include "value.hpp"

namespace lox
{
    template
    <
        typename TKey,
        typename TValue
    >
    struct entry
    {
        TKey key;
        TValue value;

        entry(TKey key, TValue value)
        {
            this->key = key;
            this->value = value;
        }

        bool deleted() const
        {
            return m_deleted;
        }

        static entry tombstone()
        {
            entry e{ {}, {} };
            e.m_deleted = true;
            return e;
        }

    private:

        bool m_deleted = false;
    };

    template 
    <
        typename TKey, 
        typename TValue,
        typename TKeyHasher          = std::hash<TKey>,
        typename TKeyEqualityChecker = std::equal_to<TKey>
    >
    class table final : public collection<entry<TKey, TValue>, TKey, uint32_t>
    {
    public:

        using key_t   = TKey;
        using val_t   = TValue;
        using entry_t = entry<key_t, val_t>;
        using cap_t   = uint32_t;

    private:

        array<std::optional<entry_t>, cap_t, cap_t> m_entries;

        template <typename TKey>
        std::optional<entry_t>& find(TKey key) const
        {
            TKeyHasher hash{};

            const auto capacity = m_entries.capacity();
            auto index = static_cast<cap_t>(hash(key)) % capacity;
            
            TKeyEqualityChecker equals{};

            std::optional<entry_t>* earlier_tombstone = nullptr;

            while (true) // It should always have at least one free spot
            {
                auto& entry = const_cast<std::optional<entry_t>&>(m_entries.get(index)); // bad
                
                if (!entry)
                {
                    // key not present, return empty bucket (first tombstone found or, if none, this entry)
                    return earlier_tombstone ? *earlier_tombstone : entry;
                }
                else if (!entry->deleted() && equals(entry->key, key))
                {
                    // key present, entry not deleted
                    return entry;
                }
                else if (entry->deleted() && !earlier_tombstone)
                {
                    // first tombstone (subsequent ones are skipped)
                    earlier_tombstone = &entry;
                }

                index = (index + 1) % capacity;
            }
        }

        void adjust_capacity(cap_t new_capacity)
        {
            auto old_entries = decltype(m_entries) { new_capacity };
            std::swap(m_entries, old_entries);

            for (cap_t i = 0, max_i = std::min(m_entries.capacity(), new_capacity); i < max_i; ++i)
            {
                auto& entry = old_entries.get(i);

                if (entry && !entry->deleted())
                {
                    std::swap(entry, find(entry->key));
                }
            }
        }

    public:

        table()
            : m_entries{ grow_capacity<cap_t>(0) }
        {
        }

        table(cap_t initial_capacity)
            : m_entries{ initial_capacity > 0 ? initial_capacity : grow_capacity<cap_t>(0) }
        {
        }

        ///
        /// Returns the number of entries in the table (both real and tombstones).
        ///
        cap_t count() const final
        {
            cap_t count = 0;

            for (cap_t i = 0, max_i = m_entries.capacity(); i < max_i; ++i)
            {
                if (m_entries.get(i)) ++count;
            }

            return count;
        }

        entry_t& get(key_t key) final
        {
            auto& entry = find(key);

            if (!entry) entry.emplace(key, val_t{});

            return *entry;
        }

        std::optional<entry_t>& maybe_get(key_t key)
        {
            return find(key);
        }

        const entry_t& get(key_t key) const final
        {
            auto& entry = find(key);
            
            if (!entry) throw std::out_of_range{ "Key not found in table" };

            return *entry;
        }

        const std::optional<entry_t>& maybe_get(key_t key) const
        {
            return find(key);
        }

        key_t add(entry_t key_val_pair) final
        {
            const auto key = key_val_pair.key;

            find(key) = key_val_pair;
            
            return key;
        }

        std::optional<entry_t> remove(key_t key)
        {
            auto& entry = find(key);

            if (entry)
            {
                auto aux = entry_t::tombstone();

                std::swap(*entry, aux);

                return aux;
            }

            return {};
        }
    };
};
