#pragma once

#include <core/type_traits.hpp>

#include <cassert>
#include <forward_list>
#include <memory>
#include <type_traits>

namespace csb
{
    namespace impl
    {
        template <typename T> struct node
        {
            static_assert(is_regular_v<T>);

            using next_t = std::unique_ptr<node<T>>;
            template <typename... Args>
            node(std::unique_ptr<node<T>> next, Args &&... args)
                  : value(std::forward<Args>(args)...), next(std::move(next))
            {
            }
            T value;
            next_t next;
        };

        template <typename T> using next_t = typename node<T>::next_t;
    } // namespace impl
    template <typename T> class singly_linked_list_iterator;
    template <typename T> class const_singly_linked_list_iterator;
    template <typename T> class singly_linked_list;

    template <typename T>
    singly_linked_list_iterator<T> before(singly_linked_list<T> &list,
                                          singly_linked_list_iterator<T> pos);

    template <typename T>
    singly_linked_list_iterator<T> last(singly_linked_list<T> &list);

    template <typename T> class singly_linked_list_iterator
    {
        static_assert(is_regular_v<T>);

      public:
        using value_t = T;

        using difference_type = std::ptrdiff_t;
        using value_type = value_t;
        using pointer = std::add_pointer_t<value_type>;
        using reference = std::add_lvalue_reference_t<value_type>;
        using iterator_category = std::forward_iterator_tag;

        singly_linked_list_iterator() noexcept = default;
        singly_linked_list_iterator(impl::node<T> *ndptr) noexcept : _ptr(ndptr)
        {
        }
        singly_linked_list_iterator(impl::next_t<T> &ndptr) noexcept
              : _ptr(ndptr.get())
        {
        }

        singly_linked_list_iterator &operator++() noexcept
        {
            _ptr = next().get();
            return *this;
        }

        singly_linked_list_iterator operator++(int) noexcept
        {
            auto prev = *this;
            _ptr = next().get();
            return prev;
        }

        T &operator*() const noexcept { return _ptr->value; }

        T *operator->() const noexcept { return &_ptr->value; }

        friend bool operator==(singly_linked_list_iterator const &l,
                               singly_linked_list_iterator const &r)
        {
            return l._ptr == r._ptr;
        }

        friend bool operator!=(singly_linked_list_iterator const &l,
                               singly_linked_list_iterator const &r)
        {
            return !(l == r);
        }

      private:
        auto const &next() const { return _ptr->next; }

        auto &next() { return _ptr->next; }

        void next(std::unique_ptr<impl::node<T>> n)
        {
            _ptr->next = std::move(n);
        }

        impl::node<T> *_ptr = nullptr;

        friend class singly_linked_list<T>;
        friend class const_singly_linked_list_iterator<T>;
    };

    template <typename T> class const_singly_linked_list_iterator
    {
        static_assert(is_regular_v<T>);

      public:
        using value_t = T;

        using difference_type = std::ptrdiff_t;
        using value_type = value_t;
        using pointer = std::add_pointer_t<const value_type>;
        using reference = std::add_lvalue_reference_t<const value_type>;
        using iterator_category = std::forward_iterator_tag;

        const_singly_linked_list_iterator() noexcept = default;
        const_singly_linked_list_iterator(impl::node<T> *ndptr) noexcept
              : _ptr(ndptr)
        {
        }
        const_singly_linked_list_iterator(impl::next_t<T> const &ndptr) noexcept
              : _ptr(ndptr.get())
        {
        }
        const_singly_linked_list_iterator(
            singly_linked_list_iterator<T> it) noexcept
              : _ptr(it._ptr)
        {
        }

        const_singly_linked_list_iterator &operator++() noexcept
        {
            _ptr = next().get();
            return *this;
        }

        const_singly_linked_list_iterator operator++(int) noexcept
        {
            auto prev = *this;
            _ptr = next().get();
            return prev;
        }

        T const &operator*() const noexcept { return _ptr->value; }

        T const *operator->() const noexcept { return &_ptr->value; }

        friend bool operator==(const_singly_linked_list_iterator const &l,
                               const_singly_linked_list_iterator const &r)
        {
            return l._ptr == r._ptr;
        }

        friend bool operator!=(const_singly_linked_list_iterator const &l,
                               const_singly_linked_list_iterator const &r)
        {
            return !(l == r);
        }

      private:
        auto const &next() const { return _ptr->next; }

        impl::node<T> const *_ptr = nullptr;
    };

    template <typename T> class singly_linked_list
    {
        static_assert(is_regular_v<T>);

      public:
        using iterator_t = singly_linked_list_iterator<T>;
        using const_iterator_t = const_singly_linked_list_iterator<T>;
        using size_type = std::size_t;
        using value_t = T;
        using value_type = value_t;

        singly_linked_list() noexcept = default;

        singly_linked_list(singly_linked_list const &other) : _size(other._size)
        {
            auto *next = &_head;

            for (auto const &elem : other)
            {
                *next = make_node(nullptr, elem);
                next = &(next->get()->next);
            }
        }

        singly_linked_list(singly_linked_list &&) noexcept = default;
        singly_linked_list &operator=(singly_linked_list const &other)
        {
            _size = other._size;
            auto *next = &_head;
            for (auto const &elem : other)
            {
                if (*next)
                {
                    next->get()->value = elem;
                }
                else
                {
                    *next = make_node(nullptr, elem);
                }
                next = &(next->get()->next);
            }
            // make sure tail is tail
            next->reset();
            return *this;
        }

        singly_linked_list &operator=(singly_linked_list &&) noexcept = default;

        constexpr bool is_empty() const noexcept { return _size == 0; }
        constexpr bool empty() const noexcept { return is_empty(); }

        constexpr size_type size() const noexcept { return _size; }

        iterator_t begin() noexcept { return _head.get(); }

        iterator_t end() noexcept { return {nullptr}; }

        const_iterator_t begin() const noexcept { return _head.get(); }

        const_iterator_t end() const noexcept { return {nullptr}; }

        iterator_t push_front(T t)
        {
            ++_size;
            _head = make_node(std::move(_head), std::move(t));
            return _head.get();
        }

        iterator_t push_back(T t)
        {
            if (!_head.get())
                return push_front(std::move(t));

            ++_size;
            auto front = _head.get();

            while (front->next)
                front = front->next.get();

            front->next = make_node(nullptr, std::move(t));
            return front->next.get();
        }

        template <typename R, typename = std::enable_if_t<is_range_v<R>>>
        iterator_t append(R &&r)
        {
            using std::begin;
            using std::end;
            const auto size = std::distance(begin(r), end(r));
            if (size == 0)
                return this->end();

            auto *next = _head ? &last(*this).next() : &_head;
            const auto appended_beg = next;

            for (auto const &rmem : r)
            {
                *next = make_node(nullptr, rmem);
                next = &next->get()->next;
            }

            _size += size;
            return *appended_beg;
        }

        iterator_t insert(iterator_t pos, T t)
        {
            if (pos == begin())
            {
                _size++;
                _head = make_node(std::move(_head), t);
                return begin();
            }

            return insert_after(before(*this, pos), std::move(t));
        }

        iterator_t insert_after(iterator_t pos, T value)
        {
            assert(pos != end());
            assert(!is_empty());

            ++_size;
            pos.next(make_node(std::move(pos.next()), value));
            return ++pos;
        }

        void pop_front()
        {
            if (!_head)
                return;

            --_size;
            _head = std::move(_head->next);
        }

        void pop_back()
        {
            if (_size <= 1)
            {
                _size = 0;
                _head = nullptr;
                return;
            }

            --_size;
            auto it = this->begin();
            auto trailing = it++;

            while (it.next().get() != this->end())
            {
                trailing = it++;
            }

            trailing._ptr->next = std::move(it._ptr->next);
        }

        iterator_t erase(iterator_t pos)
        {
            if (pos == begin())
            {
                pop_front();
                return begin();
            }

            auto b4 = before(*this, pos);
            auto &after = b4.next()->next;

            b4.next(std::move(after));
            --_size;

            return b4.next();
        }

        void reverse()
        {
            auto prev = impl::next_t<T>();
            auto curr = std::move(_head);
            auto next = impl::next_t<T>();

            while (curr)
            {
                next = std::move(curr->next);
                curr->next = std::move(prev);
                prev = std::move(curr);
                curr = std::move(next);
            }

            _head = std::move(prev);
        }

        void merge(singly_linked_list other)
        {
            impl::next_t<T> new_begin;

            auto tail = &new_begin;
            auto a = &_head;
            auto b = &other._head;

            while (true)
            {
                if (!(*a))
                {
                    *tail = std::move(*b);
                    break;
                }
                else if (!(*b))
                {
                    *tail = std::move(*a);
                    break;
                }
                if (b->get()->value < a->get()->value)
                {
                    *tail = std::move(*b);
                    *b = std::move(tail->get()->next);
                    tail->get()->next = nullptr;
                }
                else
                {
                    *tail = std::move(*a);
                    *a = std::move(tail->get()->next);
                    tail->get()->next = nullptr;
                }

                tail = &(tail->get()->next);
            }

            _head = std::move(new_begin);
        }

        friend bool operator==(singly_linked_list const &l,
                               singly_linked_list const &r)
        {
            auto lpos = l.begin();
            auto rpos = r.begin();

            while (lpos != l.end() && rpos != r.end())
            {
                if (*lpos != *rpos)
                    return false;

                ++lpos;
                ++rpos;
            }

            return lpos == l.end() && rpos == r.end();
        }

        friend bool operator!=(singly_linked_list const &l,
                               singly_linked_list const &r)
        {
            return !(l == r);
        }

      private:
        template <typename... Args>
        auto make_node(impl::next_t<T> next, Args &&... args)
        {
            return std::make_unique<impl::node<T>>(std::move(next),
                                                   std::forward<Args>(args)...);
        }

        impl::next_t<T> _head = nullptr;
        size_type _size = 0;
    };

    template <typename T> class singly_linked_list<T &>
    {
        static_assert("lists of references are not allowed");
    };

    template <typename T>
    singly_linked_list_iterator<T> before(singly_linked_list<T> &list,
                                          singly_linked_list_iterator<T> pos)
    {
        auto trailing = pos;
        auto it = list.begin();
        while (it != pos)
            trailing = it++;
        return trailing;
    }

    template <typename T>
    singly_linked_list_iterator<T> last(singly_linked_list<T> &list)
    {
        return before(list, list.end());
    }

    template <typename T, typename Fn, typename = std::is_invocable<Fn, T>>
    void apply_in_reverse_recursive(singly_linked_list_iterator<T> start,
                                    singly_linked_list_iterator<T> end,
                                    Fn const &fn)
    {
        if (start == end)
            return;

        auto it = start++;
        apply_in_reverse(start, end, fn);
        fn(*it);
    }

    template <typename T, typename Fn, typename = std::is_invocable<Fn, T>>
    void apply_in_reverse_iterative(singly_linked_list_iterator<T> start,
                                    singly_linked_list_iterator<T> end,
                                    Fn const &fn)
    {
        auto last = end;
        auto current = end;

        while (start != last)
        {
            current = start;
            auto next = current;
            while (++next != last)
            {
                ++current;
                next = current;
            }
            fn(*current);
            last = current;
        }
    }

    template <typename T, typename Fn, typename = std::is_invocable<Fn, T>>
    void apply_in_reverse(singly_linked_list_iterator<T> start,
                          singly_linked_list_iterator<T> end, Fn const &fn)
    {
        apply_in_reverse_iterative(start, end, fn);
    }
} // namespace csb