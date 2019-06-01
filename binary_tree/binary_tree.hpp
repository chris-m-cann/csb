#ifndef CSB_BINARY_TREE_HPP
#define CSB_BINARY_TREE_HPP

#include <core/type_traits.hpp>
#include <memory>
#include <queue>
#include <type_traits>

namespace csb
{
    template <typename T> class binary_tree;

    namespace impl
    {

        template <typename T> struct binary_tree_node
        {

            ~binary_tree_node() = default;

            void add(std::unique_ptr<binary_tree_node> &v)
            {
                if (v->t <= t)
                {
                    if (left == nullptr)
                    {
                        left = std::move(v);
                        left->parent = this;
                    }
                    else
                    {
                        left->add(v);
                    }
                }
                else
                {
                    if (right == nullptr)
                    {
                        right = std::move(v);
                        right->parent = this;
                    }
                    else
                    {
                        right->add(v);
                    }
                }
            }

            binary_tree_node *find(T const &target)
            {
                if (t == target)
                {
                    return this;
                }

                if (t < target)
                {
                    if (right == nullptr)
                    {
                        return nullptr;
                    }
                    else
                    {
                        return right->find(target);
                    }
                }
                else
                {
                    if (left == nullptr)
                    {
                        return nullptr;
                    }
                    else
                    {
                        return left->find(target);
                    }
                }
            }

            binary_tree_node const *find(T const &target) const
            {
                return const_cast<binary_tree_node *>(this)->find(target);
            }

            template <typename Callable>
            void inorder_traverse(Callable const &visiter) const
            {
                if (left != nullptr)
                {
                    left->inorder_traverse(visiter);
                }

                visiter(t);

                if (right != nullptr)
                {
                    right->inorder_traverse(visiter);
                }
            }

            template <typename Callable>
            void preorder_traverse(Callable const &visiter) const
            {

                visiter(t);

                if (left != nullptr)
                {
                    left->inorder_traverse(visiter);
                }

                if (right != nullptr)
                {
                    right->inorder_traverse(visiter);
                }
            }

            template <typename Callable>
            void postorder_traverse(Callable const &visiter) const
            {
                if (left != nullptr)
                {
                    left->inorder_traverse(visiter);
                }

                if (right != nullptr)
                {
                    right->inorder_traverse(visiter);
                }

                visiter(t);
            }

            T t;
            std::unique_ptr<binary_tree_node> left = nullptr;
            std::unique_ptr<binary_tree_node> right = nullptr;
            binary_tree_node *parent = nullptr;

            explicit binary_tree_node(T &&v, binary_tree_node *parent = nullptr)
                  : t(std::move(v)),
                    left(nullptr),
                    right(nullptr),
                    parent(parent)
            {
            }
        };

        template <typename T>
        impl::binary_tree_node<T> *leftmost(impl::binary_tree_node<T> *n)
        {
            while (n && n->left)
            {
                n = n->left.get();
            }
            return n;
        }

        template <typename T>
        impl::binary_tree_node<T> *rightmost(impl::binary_tree_node<T> *n)
        {
            while (n && n->right)
            {
                n = n->right.get();
            }
            return n;
        }

        template <typename T>
        class binary_tree_iterator
              : public std::iterator<std::forward_iterator_tag, T>
        {
          public:
            binary_tree_iterator &operator++()
            {

                if (np->right != nullptr)
                {
                    // if you have a right subtree then next successor is the
                    // leftmost member of that tree
                    np = leftmost(np->right.get());
                }
                else if (np == np->parent->left.get())
                {
                    // if ive just visited the left binary_tree_node the parent
                    // is next in order
                    np = np->parent;
                }
                else
                {
                    // we are coming back up from a right subtree so need to
                    // keep going up until either we reach the root
                    // (parent==nullptr) or until we are no longer coming up
                    // from the right
                    auto trail = np;
                    np = np->parent;

                    while (np && np->right.get() == trail)
                    {
                        trail = np;
                        np = np->parent;
                    }
                }

                return *this;
            }

            binary_tree_iterator const operator++(int)
            {
                auto cpy = *this;
                ++(*this);
                return cpy;
            }

            binary_tree_iterator const &operator--()
            {

                if (np == nullptr)
                {
                    np = rightmost(root);
                }
                else if (np->left != nullptr)
                {
                    np = rightmost(np->left.get());
                }
                else if (np == np->parent->right.get())
                {
                    np = np->parent;
                }
                else
                {
                    auto trail = np;
                    np = np->parent;

                    while (np && np->left.get() == trail)
                    {
                        trail = np;
                        np = np->parent;
                    }
                }
                return *this;
            }

            binary_tree_iterator const operator--(int)
            {
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            T const &operator*() const { return np->t; }

          private:
            explicit binary_tree_iterator(binary_tree_node<T> *p,
                                          binary_tree_node<T> *root)
                  : np(p), root(root)
            {
            }

            binary_tree_node<T> *np = nullptr;
            binary_tree_node<T> *root = nullptr;

            friend class binary_tree<T>;

            friend bool operator==(binary_tree_iterator const &l,
                                   binary_tree_iterator const &r)
            {
                return l.np == r.np;
            }

            friend bool operator!=(binary_tree_iterator const &l,
                                   binary_tree_iterator const &r)
            {
                return !(l == r);
            }
        };

    } // namespace impl

    template <typename T> class binary_tree
    {
      public:
        static_assert(is_totally_ordered_v<T>,
                      "T must support operator < in order for binary tree to "
                      "function properly");

        using const_iterator = impl::binary_tree_iterator<T>;

        binary_tree() = default;
        ~binary_tree() = default;

        //        template<typename V, typename =
        //        std::enable_if_t<std::is_copy_constructible_v<V>>>
        binary_tree(binary_tree const &other)
        {
            static_assert(std::is_copy_constructible_v<T>);
            other.breadth_first_traverse([this](T const &t) { add(t); });
        }

        binary_tree(binary_tree &&other) noexcept
              : root(std::move(other.root)),
                _size(std::exchange(other._size, 0))
        {
        }

        //        template<typename =
        //        std::enable_if_t<std::is_copy_constructible_v<T>>>
        binary_tree &operator=(binary_tree const &other)
        {
            static_assert(std::is_copy_assignable_v<T>);
            binary_tree tmp(other);
            *this = std::move(tmp);
            return *this;
        }

        binary_tree &operator=(binary_tree &&other) noexcept
        {
            root = std::move(other.root);
            _size = std::exchange(other._size, 0);
            return *this;
        }

        /*implicit*/ binary_tree(std::initializer_list<T> list)
        {
            for (auto e : list)
            {
                if constexpr (std::is_move_constructible_v<T>)
                {
                    add(std::move(e));
                }
                else
                {
                    add(e);
                }
            }
        }

        template <
            template <typename, typename...> class Container, typename... Args,
            typename = std::enable_if_t<is_range_v<Container<T, Args...>>>,
            typename = std::enable_if_t<
                !is_same_or_derived_v<binary_tree, Container<T, Args...>>>,
            typename = std::enable_if_t<std::is_copy_constructible_v<T>>>
        explicit binary_tree(Container<T, Args...> &&container)
        {
            for (auto &e : container)
            {
                if constexpr (std::is_rvalue_reference_v<decltype(container)>)
                {
                    add(std::move(e));
                }
                else
                {
                    add(e);
                }
            }
        }

        friend bool operator==(binary_tree const &l, binary_tree const &r)
        {
            return l._size == r._size &&
                   std::equal(l.begin(), l.end(), r.begin());
        }

        friend bool operator!=(binary_tree const &l, binary_tree const &r)
        {
            return !(l == r);
        }

        void add(T t)
        {
            auto n = std::make_unique<impl::binary_tree_node<T>>(std::move(t));

            if (root == nullptr)
            {
                root = std::move(n);
            }
            else
            {
                root->add(n);
            }
            ++_size;
        }

        void erase(T const &t) { root = erase(root, t); }

        bool contains(T const &t) const { return find(t) != end(); }

        const_iterator find(T const &t) const
        {
            if (root == nullptr)
            {
                return end();
            }
            else
            {
                return impl::binary_tree_iterator(root->find(t), root.get());
            }
        }

        template <typename Callable>
        void inorder_traverse(Callable const &visiter) const
        {
            if (root != nullptr)
            {
                root->inorder_traverse(visiter);
            }
        }

        template <typename Callable>
        void breadth_first_traverse(Callable const &visiter) const
        {
            if (root == nullptr)
            {
                return;
            }

            std::queue<impl::binary_tree_node<T> *> q;
            q.push(root.get());

            while (!q.empty())
            {
                auto n = q.front();
                q.pop();

                visiter(n->t);
                if (n->left != nullptr)
                {
                    q.push(n->left.get());
                }
                if (n->right != nullptr)
                {
                    q.push(n->right.get());
                }
            }
        }

        bool is_empty() const { return _size == 0; }

        std::size_t size() const { return _size; }

        const_iterator begin() const
        {
            return const_iterator(impl::leftmost(root.get()), root.get());
        }

        const_iterator end() const
        {
            return const_iterator(nullptr, root.get());
        }

      private:
        std::unique_ptr<impl::binary_tree_node<T>>
        erase(std::unique_ptr<impl::binary_tree_node<T>> &parent, T const &t)
        {
            if (parent == nullptr)
            {
                return nullptr;
            }

            if (t < parent->t)
            {
                // target is in left subtree if present so erase from left
                // subtree
                parent->left = erase(parent->left, t);
            }
            else if (parent->t < t)
            {
                // target is in right subtree if present so erase from right
                // subtree
                parent->right = erase(parent->right, t);
            }
            else
            {
                // target is found

                // 1 child or no children
                if (parent->left == nullptr)
                {
                    // we know we are removing a ode and not going through any
                    // other calls to erase
                    --_size;
                    return std::move(parent->right);
                }
                else if (parent->right == nullptr)
                {
                    // we know we are removing a ode and not going through any
                    // other calls to erase
                    --_size;
                    return std::move(parent->left);
                }
                else
                {
                    // both children
                    auto next = impl::leftmost(parent->right.get());
                    parent->t = next->t;
                    parent->right = erase(parent->right, parent->t);
                }
            }

            return std::move(parent);
        }

        std::unique_ptr<impl::binary_tree_node<T>> root = nullptr;
        std::size_t _size = 0;
    };
} // namespace csb

#endif // CSB_BINARY_TREE_HPP
