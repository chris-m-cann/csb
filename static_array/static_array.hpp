#pragma once

#include <core/concepts.hpp>

#include <iostream>

namespace csb
{
    template <auto> struct idx_t
    {
    };

    template <ccept::Regular T, std::size_t n> class static_array
    {
      public:
        using value_type = T;
        using value_t = T;
        using size_type = std::size_t;
        using reference = T &;
        using const_reference = T const &;
        using pointer = value_type *;
        using const_pointer = value_type const *;
        using iterator = T *;
        using const_iterator = T const *;
        static constexpr size_type size_v = n;

        constexpr static_array() noexcept(
            std::is_nothrow_default_constructible_v<value_type>) = default;

        constexpr static_array(static_array const &) noexcept(
            std::is_nothrow_copy_constructible_v<value_type>) = default;
        constexpr static_array(static_array &&) noexcept(
            std::is_nothrow_move_constructible_v<value_type>) = default;
        constexpr static_array &operator=(static_array const &) noexcept(
            std::is_nothrow_copy_assignable_v<value_type>) = default;
        constexpr static_array &operator=(static_array &&) noexcept(
            std::is_nothrow_move_assignable_v<value_type>) = default;

        ~static_array() noexcept(std::is_nothrow_destructible_v<value_type>) =
            default;

        // should I offer this noexpetness in case I end up using lazy
        // initialization??
        constexpr static_array(value_type const &all) noexcept(
            std::is_nothrow_default_constructible_v<value_type>
                &&std::is_nothrow_copy_assignable_v<value_type>)
              : _store{}
        {
            fill(all);
        }

        template <ccept::Range R>
        constexpr static_array(R const &rng) : _store{}
        {
            init(rng);
        }

        constexpr static_array(std::initializer_list<T> const &il) noexcept(
            std::is_nothrow_default_constructible_v<value_type>
                &&std::is_nothrow_copy_assignable_v<value_type>)
              : _store{}
        {
            init(il);
        }

        constexpr const_reference operator[](size_type i) const noexcept
        {
            return _store[i];
        }
        template <auto i>
        constexpr const_reference operator[](idx_t<i>) const noexcept
        {
            static_assert(i > 0 && i < n, "out of bounds");
            return _store[i];
        }

        constexpr reference operator[](size_type i) noexcept
        {
            return _store[i];
        }
        template <auto i> constexpr reference operator[](idx_t<i>) noexcept
        {
            static_assert(i > 0 && i < n, "out of bounds");
            return _store[i];
        }

        constexpr const_reference at(size_type i) const
        {
            return const_cast<static_array const &>(*this).operator[](i);
        }
        constexpr reference at(size_type i)
        {
            if (i >= n)
                throw std::out_of_range("static_array::at() range error");
            return _store[i];
        }

        constexpr const_iterator begin() const noexcept
        {
            return std::begin(_store);
        }
        constexpr const_iterator end() const noexcept
        {
            return std::end(_store);
        }

        constexpr iterator begin() noexcept { return std::begin(_store); }
        constexpr iterator end() noexcept { return std::end(_store); }

        constexpr reference front() noexcept { return _store[0]; }
        constexpr const_reference front() const noexcept
        {
            return const_cast<static_array const &>(*this).front();
        }

        constexpr reference back() noexcept { return _store[n - 1]; }
        constexpr const_reference back() const noexcept
        {
            return const_cast<static_array const &>(*this).back();
        }

        constexpr bool is_empty() const noexcept { return false; }
        constexpr bool empty() const noexcept { return is_empty(); }

        constexpr size_type size() const noexcept { return n; }

        template <ccept::Range R> constexpr void assign(R const &r) { init(r); }

        constexpr void fill(value_type const &with) noexcept(
            std::is_nothrow_copy_assignable_v<value_type>)
        {
            std::fill_n(this->begin(), n, with);
        }

        friend bool operator==(static_array const &l,
                               static_array const &r) noexcept
        {
            return std::equal(l.begin(), l.end(), r.begin());
        }
        friend bool operator!=(static_array const &l,
                               static_array const &r) noexcept
        {
            return !(r == l);
        }

      private:
        template <ccept::Range R> constexpr void init(R const &rng)
        {
            using std::begin;
            using std::end;
            auto me = this->begin();
            for (auto rit = begin(rng); me != this->end() && rit != end(rng);
                 ++me, ++rit)
            {
                *me = *rit;
            }
        }

        T _store[n];
    };
}