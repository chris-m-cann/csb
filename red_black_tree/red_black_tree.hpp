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

        template <typename T>
        bool is_red(binary_tree_node<T, red_black_node_meta_data> const *node)
        {
            return node != nullptr &&
                   node->metadata().colour == impl::Colour::Red;
        }

        template <typename T>
        bool is_black(binary_tree_node<T, red_black_node_meta_data> const *node)
        {
            return !is_red(node);
        }

        template <typename T>
        bool is_root(binary_tree_node<T, red_black_node_meta_data> const *node)
        {
            return node != nullptr && node->parent == nullptr;
        }

        impl::Colour flipped(impl::Colour colour)
        {
            return colour == impl::Colour::Red ? impl::Colour::Black
                                               : impl::Colour::Red;
        }

        struct red_black_tree_balancing
        {
            using node_metadata_type = red_black_node_meta_data;

            template <typename T>
            using node_type = binary_tree_node<T, red_black_node_meta_data>;

            template <typename T>
            static std::unique_ptr<node_type<T>>
            balance(std::unique_ptr<node_type<T>> root, node_type<T> *node)
            {
                auto newRoot = balance_impl(std::move(root), node);
                newRoot->metadata().colour = Colour::Black;
                return std::move(newRoot);
            }

            template <typename T>
            static std::unique_ptr<node_type<T>>
            erase_node(std::unique_ptr<node_type<T>> root, node_type<T> &target)
            {
                // basically we ensure that the node to be deleted has at most
                // one child
                if (target.left != nullptr && target.right != nullptr)
                {
                    auto newTarget = leftmost(target.right.get());
                    target.t = newTarget->t;
                    return erase_node_impl(std::move(root), *newTarget);
                }
                else
                {
                    return erase_node_impl(std::move(root), target);
                }
            }

          private:
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

            /*
             * s is black an has at least 1 red child, r
             * rotate based on the position of s and r
             */
            template <typename T>
            static std::unique_ptr<node_type<T>>
            case_1(std::unique_ptr<node_type<T>> root, node_type<T> *,
                   node_type<T> *parent, node_type<T> *sibling)
            {
                auto &strong_parent = [&]() -> std::unique_ptr<node_type<T>> & {
                    if (parent->parent == nullptr)
                    {
                        return root;
                    }
                    else if (is_left_child(*parent))
                    {
                        return parent->parent->left;
                    }
                    else
                    {
                        return parent->parent->right;
                    }
                }();

                auto s_left = is_left_child(*sibling);
                if (s_left)
                {
                    if (is_red(sibling->left.get()))
                    {
                        // left left case
                        sibling->left->metadata().colour =
                            sibling->metadata().colour;
                        sibling->metadata().colour = parent->metadata().colour;
                        parent->metadata().colour = impl::Colour::Black;
                        strong_parent = right_rotate(std::move(strong_parent));
                    }
                    else
                    {
                        // left right case
                        sibling->right->metadata().colour =
                            parent->metadata().colour;
                        parent->left = left_rotate(std::move(parent->left));
                        parent->metadata().colour = impl::Colour::Black;
                        strong_parent = right_rotate(std::move(strong_parent));
                    }
                }
                else
                {
                    if (is_red(sibling->left.get()))
                    {
                        // right left case
                        sibling->left->metadata().colour =
                            parent->metadata().colour;
                        parent->right = right_rotate(std::move(parent->right));
                        parent->metadata().colour = impl::Colour::Black;
                        strong_parent = left_rotate(std::move(strong_parent));
                    }
                    else
                    {
                        // right right case
                        sibling->right->metadata().colour =
                            sibling->metadata().colour;
                        sibling->metadata().colour = parent->metadata().colour;
                        parent->metadata().colour = impl::Colour::Black;
                        strong_parent = left_rotate(std::move(strong_parent));
                    }
                }
                return std::move(root);
            }

            /*
             * s is black and both its children are black
             * recolour sibling red
             * if parent is red, colour it black and your done
             * else recur on p
             */
            template <typename T>
            static std::unique_ptr<node_type<T>>
            case_2(std::unique_ptr<node_type<T>> root, node_type<T> &parent,
                   node_type<T> *sibling)
            {

                if (sibling != nullptr)
                {
                    sibling->metadata().colour = impl::Colour::Red;
                }

                if (parent.metadata().colour == impl::Colour::Red)
                {
                    parent.metadata().colour = impl::Colour::Black;
                    return std::move(root);
                }
                else
                {
                    if (parent.parent == nullptr)
                    {
                        return case_4(std::move(root));
                    }
                    // recur
                    auto &newSibling = (is_left_child(parent))
                                           ? parent.parent->right
                                           : parent.parent->left;
                    return fix_double_black_impl(std::move(root),
                                                 &parent,
                                                 parent.parent,
                                                 newSibling.get());
                }
            }

            /*
             * sibling is red
             * flip p and s's colours
             * rotate around s
             * then recur
             */
            template <typename T>
            static std::unique_ptr<node_type<T>>
            case_3(std::unique_ptr<node_type<T>> root, node_type<T> *,
                   node_type<T> *parent, node_type<T> *sibling)
            {

                std::swap(sibling->metadata().colour,
                          parent->metadata().colour);

                auto &strong_parent = [&]() -> std::unique_ptr<node_type<T>> & {
                    if (parent->parent == nullptr)
                    {
                        return root;
                    }
                    else if (is_left_child(*parent))
                    {
                        return parent->parent->left;
                    }
                    else
                    {
                        return parent->parent->right;
                    }
                }();

                if (is_left_child(*sibling))
                {
                    strong_parent = right_rotate(std::move(strong_parent));
                    return fix_double_black_impl(
                        std::move(root),
                        strong_parent->right->right.get(),
                        strong_parent->right.get(),
                        strong_parent->right->left.get());
                }
                else
                {
                    strong_parent = left_rotate(std::move(strong_parent));
                    return fix_double_black_impl(
                        std::move(root),
                        strong_parent->left->left.get(),
                        strong_parent->left.get(),
                        strong_parent->left->right.get());
                }
            }

            /*
             * v is root, set black and return it
             */
            template <typename T>
            static std::unique_ptr<node_type<T>>
            case_4(std::unique_ptr<node_type<T>> root)
            {
                if (root != nullptr)
                {
                    root->metadata().colour = impl::Colour::Black;
                }
                return std::move(root);
            }

            template <typename T, typename Metadata>
            static std::unique_ptr<binary_tree_node<T, Metadata>>
            fix_double_black_impl(
                std::unique_ptr<binary_tree_node<T, Metadata>> root,
                binary_tree_node<T, Metadata> *doubleBlack,
                binary_tree_node<T, Metadata> *parent,
                binary_tree_node<T, Metadata> *sibling)
            {

                if (parent == nullptr)
                {
                    return case_4(std::move(root));
                }

                if (is_red(sibling))
                {
                    return case_3(
                        std::move(root), doubleBlack, parent, sibling);
                }

                if (sibling == nullptr || (is_black(sibling->left.get()) &&
                                           is_black(sibling->right.get())))
                { // i.e. sibling is black
                    return case_2(std::move(root), *parent, sibling);
                }

                return case_1(std::move(root), doubleBlack, parent, sibling);
            }

            template <typename T, typename Metadata>
            static std::unique_ptr<binary_tree_node<T, Metadata>>
            fix_double_black(
                std::unique_ptr<binary_tree_node<T, Metadata>> root,
                binary_tree_node<T, Metadata> &target,
                std::unique_ptr<binary_tree_node<T, Metadata>> child = nullptr)
            {
                auto parent = target.parent;
                auto doubleBlack = child.get();

                binary_tree_node<T, Metadata> *sibling = nullptr;
                if (parent == nullptr)
                {
                    sibling = nullptr;
                }
                else if (is_left_child(target))
                {
                    sibling = target.parent->right.get();
                }
                else
                {
                    sibling = target.parent->left.get();
                }

                root = detach(std::move(root), target, std::move(child));

                if (root == nullptr)
                {
                    return nullptr;
                }
                else
                {
                    return fix_double_black_impl(
                        std::move(root), doubleBlack, parent, sibling);
                }
            }

            template <typename T>
            static std::unique_ptr<node_type<T>>
            erase_node_impl(std::unique_ptr<node_type<T>> root,
                            node_type<T> &target)
            {
                auto &child =
                    target.left != nullptr ? target.left : target.right;

                // if red then just delete
                if (is_red(&target))
                {
                    return detach(std::move(root), target, std::move(child));
                }

                // child is red, colour black and replace target with it
                if (is_red(child.get()))
                {
                    child->metadata().colour = Colour::Black;
                    return detach(std::move(root), target, std::move(child));
                }

                return fix_double_black(
                    std::move(root), target, std::move(child));
            }
        };
    } // namespace impl

    template <typename T>
    using red_black_tree = binary_tree<T, impl::red_black_tree_balancing>;

} // namespace csb

#endif // CSB_RED_BLACK_TREE_HPP
