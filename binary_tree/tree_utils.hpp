#ifndef CSB_TREE_UTILS_HPP
#define CSB_TREE_UTILS_HPP

#include <memory>

namespace csb
{
    template <typename T> struct tree_node
    {
        T t;
        std::unique_ptr<tree_node> left = nullptr;
        std::unique_ptr<tree_node> right = nullptr;
        tree_node *parent = nullptr;

        explicit tree_node(T &&v, tree_node *parent = nullptr)
              : t(std::move(v)), left(nullptr), right(nullptr), parent(parent)
        {
        }

        friend std::unique_ptr<tree_node>
        leftRotate(std::unique_ptr<tree_node> grandparent)
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

        friend std::unique_ptr<tree_node>
        rightRotate(std::unique_ptr<tree_node> grandparent)
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

        friend std::unique_ptr<tree_node>
        leftRightRotate(std::unique_ptr<tree_node> grandparent)
        {
            grandparent->left = leftRotate(std::move(grandparent->left));
            return rightRotate(std::move(grandparent));
        }

        friend std::unique_ptr<tree_node>
        rightLeftRotate(std::unique_ptr<tree_node> grandparent)
        {
            grandparent->right = rightRotate(std::move(grandparent->right));
            return leftRotate(std::move(grandparent));
        }
    };

} // namespace csb

#endif // CSB_TREE_UTILS_HPP
