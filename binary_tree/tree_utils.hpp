#ifndef CSB_TREE_UTILS_HPP
#define CSB_TREE_UTILS_HPP

#include <memory>

namespace csb
{

    template <typename T, typename Metadata>
    struct binary_tree_node : private Metadata
    {
        using value_type = T;

        ~binary_tree_node() = default;

        Metadata &metadata() { return *this; }
        Metadata const &metadata() const { return *this; }

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
              : Metadata(),
                t(std::move(v)),
                left(nullptr),
                right(nullptr),
                parent(parent)
        {
        }

        friend std::unique_ptr<binary_tree_node>
        left_rotate(std::unique_ptr<binary_tree_node> grandparent)
        {
            auto tmp = std::move(grandparent->right);
            tmp->parent = grandparent->parent;
            grandparent->right = std::move(tmp->left);
            if (grandparent->right)
            {
                grandparent->right->parent = grandparent.get();
            }
            tmp->left = std::move(grandparent);
            tmp->left->parent = tmp.get();
            return std::move(tmp);
        }

        friend std::unique_ptr<binary_tree_node>
        right_rotate(std::unique_ptr<binary_tree_node> grandparent)
        {
            auto tmp = std::move(grandparent->left);
            tmp->parent = grandparent->parent;
            grandparent->left = std::move(tmp->right);
            if (grandparent->left)
            {
                grandparent->left->parent = grandparent.get();
            }
            tmp->right = std::move(grandparent);
            tmp->right->parent = tmp.get();
            return std::move(tmp);
        }

        friend std::unique_ptr<binary_tree_node>
        left_right_rotate(std::unique_ptr<binary_tree_node> grandparent)
        {
            grandparent->left = left_rotate(std::move(grandparent->left));
            return right_rotate(std::move(grandparent));
        }

        friend std::unique_ptr<binary_tree_node>
        right_left_rotate(std::unique_ptr<binary_tree_node> grandparent)
        {
            grandparent->right = right_rotate(std::move(grandparent->right));
            return left_rotate(std::move(grandparent));
        }
    };

    template <typename T, typename Metadata>
    binary_tree_node<T, Metadata> *leftmost(binary_tree_node<T, Metadata> *n)
    {
        while (n && n->left)
        {
            n = n->left.get();
        }
        return n;
    }

    template <typename T, typename Metadata>
    binary_tree_node<T, Metadata> *rightmost(binary_tree_node<T, Metadata> *n)
    {
        while (n && n->right)
        {
            n = n->right.get();
        }
        return n;
    }

} // namespace csb

#endif // CSB_TREE_UTILS_HPP
