#ifndef CSB_RED_BLACK_TREE_HPP
#define CSB_RED_BLACK_TREE_HPP

#include <binary_tree/binary_tree.hpp>
#include <binary_tree/tree_utils.hpp>

#include <memory>

namespace csb
{
    namespace impl
    {
        enum class Colour : unsigned char
        {
            Red,
            Black
        };

        std::ostream &operator<<(std::ostream &os, Colour const &c)
        {

            switch (c)
            {
            case impl::Colour::Black:
                os << "Colour::Black";
                break;
            case impl::Colour::Red:
                os << "Colour::Red";
                break;
            }
            return os;
        }

        struct red_black_node_meta_data
        {
            Colour colour = Colour::Red;
        };

        template <typename T, typename M>
        binary_tree_node<T, M> *find_aunt(binary_tree_node<T, M> *n)
        {
            if (n == nullptr || n->parent == nullptr ||
                n->parent->parent == nullptr)
            {
                return nullptr;
            }
            else
            {
                auto grandparent = n->parent->parent;
                if (grandparent->left.get() == n->parent)
                {
                    return grandparent->right.get();
                }
                else
                {
                    return grandparent->left.get();
                }
            }
        }

        template <typename T>
        bool
        is_left_left(binary_tree_node<T, red_black_node_meta_data> const &node)
        {
            // if node is in its parents left subtree which is in turn in its
            // parents left subtree
            return node.parent->left.get() == &node &&
                   node.parent->parent->left.get() == node.parent;
        }

        template <typename T>
        bool
        is_left_right(binary_tree_node<T, red_black_node_meta_data> const &node)
        {
            // if node is in its parents right subtree which is in turn in its
            // parents left subtree
            return node.parent->right.get() == &node &&
                   node.parent->parent->left.get() == node.parent;
        }

        template <typename T>
        bool is_right_right(
            binary_tree_node<T, red_black_node_meta_data> const &node)
        {
            // if node is in its parents right subtree which is in turn in its
            // parents right subtree
            return node.parent->right.get() == &node &&
                   node.parent->parent->right.get() == node.parent;
        }

        template <typename T>
        bool
        is_right_left(binary_tree_node<T, red_black_node_meta_data> const &node)
        {
            // if node is in its parents left subtree which is in turn in its
            // parents right subtree
            return node.parent->left.get() == &node &&
                   node.parent->parent->right.get() == node.parent;
        }

        struct red_black_tree_balancing
        {
            using node_metadata_type = red_black_node_meta_data;

            template <typename T>
            using node_type = binary_tree_node<T, red_black_node_meta_data>;

            template <typename T> static bool is_black(node_type<T> *n)
            {
                return n == nullptr || n->metadata().colour == Colour::Black;
            }

            template <typename T>
            static std::unique_ptr<node_type<T>>
            recolour(std::unique_ptr<node_type<T>> root, node_type<T> *node)
            {
                auto grandparent = node->parent->parent;
                grandparent->metadata().colour = Colour::Red;

                auto aunt = find_aunt(node);
                aunt->metadata().colour = Colour::Black;
                node->parent->metadata().colour = Colour::Black;

                return balance(std::move(root), grandparent);
            }

            template <typename T>
            static std::unique_ptr<node_type<T>>
            rotate(std::unique_ptr<node_type<T>> root, node_type<T> *node)
            {
                (void)node;
                auto grandparent = node->parent->parent;
                auto great_grandparent = grandparent->parent;
                decltype(root) *link = nullptr;
                if (great_grandparent == nullptr)
                {
                    link = &root;
                }
                else
                {
                    link = grandparent == great_grandparent->left.get()
                               ? &great_grandparent->left
                               : &great_grandparent->right;
                }
                if (is_left_left(*node))
                {
                    *link = right_rotate(std::move(*link));
                    std::swap(link->get()->metadata().colour,
                              link->get()->right->metadata().colour);
                }
                else if (is_left_right(*node))
                {
                    *link = left_right_rotate(std::move(*link));
                    std::swap(link->get()->metadata().colour,
                              link->get()->right->metadata().colour);
                }
                else if (is_right_right(*node))
                {
                    *link = left_rotate(std::move(*link));
                    std::swap(link->get()->metadata().colour,
                              link->get()->left->metadata().colour);
                }
                else
                { // must be right left
                    *link = right_left_rotate(std::move(*link));
                    std::swap(link->get()->metadata().colour,
                              link->get()->left->metadata().colour);
                }

                return std::move(root);
            }

            template <typename T>
            static std::unique_ptr<node_type<T>>
            balance_impl(std::unique_ptr<node_type<T>> root, node_type<T> *node)
            {
                // 1st node so just make sure root is black
                if (root.get() == node)
                {
                    root->metadata().colour = Colour::Black;
                    return std::move(root);
                }

                // iF parent is black then all criteria will be met
                if (is_black(node->parent))
                {
                    return std::move(root);
                }

                auto aunt = find_aunt(node);

                if (is_black(aunt))
                {
                    root = rotate(std::move(root), node);
                }
                else
                {
                    root = recolour(std::move(root), node);
                }

                return std::move(root);
            }

            template <typename T>
            static std::unique_ptr<node_type<T>>
            balance(std::unique_ptr<node_type<T>> root, node_type<T> *node)
            {
                auto newRoot = balance_impl(std::move(root), node);
                newRoot->metadata().colour = Colour::Black;
                return std::move(newRoot);
            }
        };
    } // namespace impl

    template <typename T>
    class red_black_tree : public binary_tree<T, impl::red_black_tree_balancing>
    {
    };
} // namespace csb

#endif // CSB_RED_BLACK_TREE_HPP
