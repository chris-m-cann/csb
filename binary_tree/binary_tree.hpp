#ifndef CSB_BINARY_TREE_HPP
#define CSB_BINARY_TREE_HPP

#include "tree_utils.hpp"
#include <core/type_traits.hpp>

#include <memory>
#include <queue>
#include <type_traits>

namespace csb
{
    namespace impl
    {
        struct null_balancing_policy
        {
            using node_metadata_type = null_balancing_policy;

            template <typename T>
            using node_type = binary_tree_node<T, node_metadata_type>;

            template <typename T>
            static std::unique_ptr<node_type<T>>
            balance(std::unique_ptr<node_type<T>> root, node_type<T> *node)
            {
                // empty as there is no balancing a regular bst
                (void)node;
                return std::move(root);
            }

            template <typename T>
            static std::unique_ptr<node_type<T>>
            erase_node(std::unique_ptr<node_type<T>> root, node_type<T> &target)
            {
                // regular bst deletion. no balancing needed
                auto replacement = find_replacement(target);

                // target is leaf node
                if (replacement == nullptr)
                {
                    // remove node
                    root = detach(std::move(root), target);
                }
                // target has 1 child
                else if (target.left == nullptr || target.right == nullptr)
                {
                    auto &child = target.left ? target.left : target.right;
                    root = detach(std::move(root), target, std::move(child));
                }
                else
                {
                    std::swap(target.t, replacement->t);
                    root = erase_node(std::move(root), *replacement);
                }

                return std::move(root);
            }
        };

    } // namespace impl

    template <typename T,
              typename BalancingPolicy = impl::null_balancing_policy>
    class binary_tree;

    namespace impl
    {

        template <typename Node>
        class binary_tree_iterator
              : public std::iterator<std::forward_iterator_tag,
                                     typename Node::value_type>
        {
          public:
            using value_type = typename Node::value_type;

            binary_tree_iterator &operator++()
            {

                if (np->right != nullptr)
                {
                    // if you have a right subtree then next successor is the
                    // leftmost member of that tree
                    np = leftmost(np->right.get());
                }
                else if (np->parent == nullptr)
                {
                    // if at the root and we have no right subtree then we are
                    // at end
                    np = nullptr;
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

            binary_tree_iterator operator++(int)
            {
                auto cpy = *this;
                ++(*this);
                return cpy;
            }

            binary_tree_iterator const &operator--()
            {

                // np == end the start got to rightmost of root
                if (np == nullptr)
                {
                    np = rightmost(root);
                }
                // if np has left child then nex inline is down that subtree
                else if (np->left != nullptr)
                {
                    np = rightmost(np->left.get());
                }
                // if you are root and you dont have a left subtree then we are
                // done
                else if (np->parent == nullptr)
                {
                    np = nullptr;
                }
                // if we are our parents right subtree then the parent is next
                // followed by its left subtree as they are all less than it
                else if (np == np->parent->right.get())
                {
                    np = np->parent;
                }
                // if weve come up from the left then keep going up until we
                // arnt or untill we hit the root (and are therefore done
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

            binary_tree_iterator operator--(int)
            {
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            value_type const &operator*() const { return np->t; }

            Node const &node() const { return *np; }

          private:
            explicit binary_tree_iterator(Node *p, Node *root)
                  : np(p), root(root)
            {
            }

            Node *np = nullptr;
            Node *root = nullptr;

            template <typename T, typename BP> friend class binary_tree;

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

    template <typename T, typename BalancingPolicy> class binary_tree
    {
      public:
        static_assert(is_totally_ordered_v<T>,
                      "T must support operator < in order for binary tree to "
                      "function properly");

        using node_type =
            binary_tree_node<T, typename BalancingPolicy::node_metadata_type>;
        using const_iterator = impl::binary_tree_iterator<node_type>;

        binary_tree() = default;
        ~binary_tree() = default;

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

        explicit binary_tree(std::unique_ptr<node_type> root)
              : root(std::move(root)), _size(0)
        {
            _size = std::distance(begin(), end());
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
            auto n = std::make_unique<node_type>(std::move(t));
            auto tmp = n.get();

            if (root == nullptr)
            {
                root = BalancingPolicy::balance(std::move(n), tmp);
                _size = 1;
            }
            else
            {
                if (root->add(n))
                {
                    root = BalancingPolicy::balance(std::move(root), tmp);
                    ++_size;
                }
            }
        }

        void erase(T const &t)
        {
            if (root != nullptr)
            {
                auto target = root->find(t);

                if (target != nullptr)
                {
                    --_size;
                    root =
                        BalancingPolicy::erase_node(std::move(root), *target);
                }
            }
        }

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
            breadth_first_traverse_nodes(
                [&visiter](node_type const &node) { visiter(node.t); });
        }

        template <typename Callable>
        void breadth_first_traverse_nodes(Callable const &visiter) const
        {
            if (root == nullptr)
            {
                return;
            }

            std::queue<node_type *> q;
            q.push(root.get());

            while (!q.empty())
            {
                auto n = q.front();
                q.pop();

                visiter(*n);
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
            return const_iterator(leftmost(root.get()), root.get());
        }

        const_iterator end() const
        {
            return const_iterator(nullptr, root.get());
        }

      private:
        std::unique_ptr<node_type> root = nullptr;
        std::size_t _size = 0;
    };
} // namespace csb

#endif // CSB_BINARY_TREE_HPP
