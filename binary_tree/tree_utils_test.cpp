#include "tree_utils.hpp"

#include <catch2/catch.hpp>

namespace csb
{
    struct Unit
    {
    };
    template <typename T> using tree_node = binary_tree_node<T, Unit>;

    SCENARIO("left rotate")
    {
        GIVEN("a right-heavy imbalanced tree")
        {
            WHEN("the imbalanced node is in the right subtree of it's "
                 "grandparents right subtree")
            {

                /*     5
                 *    / \
                 *   2   9
                 *      / \
                 *     6   13
                 *         / \
                 *        10  18
                 */

                auto g = std::make_unique<tree_node<int>>(5);
                g->left = std::make_unique<tree_node<int>>(2, g.get());

                auto p = std::make_unique<tree_node<int>>(9, g.get());
                p->left = std::make_unique<tree_node<int>>(6, p.get());

                auto z = std::make_unique<tree_node<int>>(13, p.get());
                z->left = std::make_unique<tree_node<int>>(10, z.get());

                z->right = std::make_unique<tree_node<int>>(18, z.get());
                p->right = std::move(z);
                g->right = std::move(p);

                THEN("a left rotation balances the tree")
                {
                    auto newG = left_rotate(std::move(g));

                    /* balanced tree
                     *
                     *      9
                     *     / \
                     *    5   13
                     *   / \  / \
                     *  2  6 10 18
                     *
                     */

                    REQUIRE(newG->t == 9);
                    REQUIRE(newG->left->t == 5);
                    REQUIRE(newG->left->left->t == 2);
                    REQUIRE(newG->left->right->t == 6);
                    REQUIRE(newG->right->t == 13);
                    REQUIRE(newG->right->left->t == 10);
                    REQUIRE(newG->right->right->t == 18);

                    REQUIRE(newG->left->parent == newG.get());
                    REQUIRE(newG->left->left->parent == newG->left.get());
                    REQUIRE(newG->left->right->parent == newG->left.get());
                    REQUIRE(newG->right->parent == newG.get());
                    REQUIRE(newG->right->left->parent == newG->right.get());
                    REQUIRE(newG->right->right->parent == newG->right.get());
                }
            }
            WHEN("There the unbalanced tree has null children")
            {
                /*     5
                 *      \
                 *       9
                 *        \
                 *         13
                 */
                auto g = std::make_unique<tree_node<int>>(5);
                auto p = std::make_unique<tree_node<int>>(9, g.get());
                auto z = std::make_unique<tree_node<int>>(13, p.get());
                p->right = std::move(z);
                g->right = std::move(p);

                THEN("a left rotation balances the tree")
                {
                    auto newG = left_rotate(std::move(g));

                    /* balanced tree
                     *
                     *      9
                     *     / \
                     *    5   13
                     *
                     */

                    REQUIRE(newG->t == 9);
                    REQUIRE(newG->left->t == 5);
                    REQUIRE(newG->right->t == 13);

                    REQUIRE(newG->left->parent == newG.get());
                    REQUIRE(newG->right->parent == newG.get());
                }
            }
        }
    }

    SCENARIO("right rotate")
    {
        GIVEN("a left-heavy imbalanced tree")
        {
            WHEN("the imbalanced node is in the left subtree of it's "
                 "grandparents left subtree")
            {

                /*       10
                 *       / \
                 *      5  20
                 *     / \
                 *    3   7
                 *   / \
                 *  1   4
                 */

                auto g = std::make_unique<tree_node<int>>(10);
                g->right = std::make_unique<tree_node<int>>(20, g.get());

                auto p = std::make_unique<tree_node<int>>(5, g.get());
                p->right = std::make_unique<tree_node<int>>(7, p.get());

                auto z = std::make_unique<tree_node<int>>(3, p.get());
                z->right = std::make_unique<tree_node<int>>(4, z.get());

                z->left = std::make_unique<tree_node<int>>(1, z.get());
                p->left = std::move(z);
                g->left = std::move(p);

                THEN("a right rotation balances the tree")
                {
                    auto newG = right_rotate(std::move(g));

                    /* balanced tree
                     *
                     *      5
                     *     /  \
                     *    3    10
                     *   / \   / \
                     *  1   4 7  20
                     *
                     */

                    REQUIRE(newG->t == 5);
                    REQUIRE(newG->left->t == 3);
                    REQUIRE(newG->left->left->t == 1);
                    REQUIRE(newG->left->right->t == 4);
                    REQUIRE(newG->right->t == 10);
                    REQUIRE(newG->right->left->t == 7);
                    REQUIRE(newG->right->right->t == 20);

                    REQUIRE(newG->left->parent == newG.get());
                    REQUIRE(newG->left->left->parent == newG->left.get());
                    REQUIRE(newG->left->right->parent == newG->left.get());
                    REQUIRE(newG->right->parent == newG.get());
                    REQUIRE(newG->right->left->parent == newG->right.get());
                    REQUIRE(newG->right->right->parent == newG->right.get());
                }
            }
            WHEN("There the unbalanced tree has null children")
            {
                /*      13
                 *      /
                 *     9
                 *    /
                 *   5
                 */
                auto g = std::make_unique<tree_node<int>>(13);
                auto p = std::make_unique<tree_node<int>>(9, g.get());
                auto z = std::make_unique<tree_node<int>>(5, p.get());
                p->left = std::move(z);
                g->left = std::move(p);

                THEN("a right rotation balances the tree")
                {
                    auto newG = right_rotate(std::move(g));

                    /* balanced tree
                     *
                     *      9
                     *     / \
                     *    5   13
                     *
                     */

                    REQUIRE(newG->t == 9);
                    REQUIRE(newG->left->t == 5);
                    REQUIRE(newG->right->t == 13);

                    REQUIRE(newG->left->parent == newG.get());
                    REQUIRE(newG->right->parent == newG.get());
                }
            }
        }
    }

    SCENARIO("left right rotate")
    {
        WHEN("a complex tree has an imbalance in the gradparent's left "
             "child's right subtree")
        {

            /*
             *     10(g)
             *    /  \
             *   5(p) 13
             *  / \
             * 1   7(z)
             *    / \
             *   6   8
             */

            auto g = std::make_unique<tree_node<int>>(10);
            g->right = std::make_unique<tree_node<int>>(13, g.get());

            auto p = std::make_unique<tree_node<int>>(5, g.get());
            p->left = std::make_unique<tree_node<int>>(1, p.get());

            auto z = std::make_unique<tree_node<int>>(7, p.get());
            z->right = std::make_unique<tree_node<int>>(8, z.get());
            z->left = std::make_unique<tree_node<int>>(6, z.get());

            p->right = std::move(z);
            g->left = std::move(p);

            THEN("a left right rotate balances the tree")
            {
                auto newG = left_right_rotate(std::move(g));

                /*
                 *      7
                 *     /  \
                 *    5    10
                 *   / \   / \
                 *  1   6  8  13
                 */

                REQUIRE(newG->t == 7);
                REQUIRE(newG->left->t == 5);
                REQUIRE(newG->left->left->t == 1);
                REQUIRE(newG->left->right->t == 6);
                REQUIRE(newG->right->t == 10);
                REQUIRE(newG->right->right->t == 13);
                REQUIRE(newG->right->left->t == 8);

                REQUIRE(newG->left->parent == newG.get());
                REQUIRE(newG->right->parent == newG.get());
                REQUIRE(newG->left->left->parent == newG->left.get());
                REQUIRE(newG->left->right->parent == newG->left.get());
                REQUIRE(newG->right->left->parent == newG->right.get());
                REQUIRE(newG->right->right->parent == newG->right.get());
            }
        }

        WHEN("a simple tree has an imbalance in the gradparent's left "
             "child's right subtree")
        {

            /*
             *     10
             *    /
             *   5
             *    \
             *     7
             */

            auto g = std::make_unique<tree_node<int>>(10);

            auto p = std::make_unique<tree_node<int>>(5, g.get());

            auto z = std::make_unique<tree_node<int>>(7, p.get());

            p->right = std::move(z);
            g->left = std::move(p);

            THEN("a left right rotate balances the tree")
            {
                auto newG = left_right_rotate(std::move(g));

                /*
                 *      7
                 *     /  \
                 *    5    10
                 */

                REQUIRE(newG->t == 7);
                REQUIRE(newG->left->t == 5);
                REQUIRE(newG->right->t == 10);

                REQUIRE(newG->left->parent == newG.get());
                REQUIRE(newG->right->parent == newG.get());
            }
        }
    }

    SCENARIO("right left rotate")
    {
        WHEN("a complex tree has an imbalance in the gradparent's right "
             "child's left subtree")
        {

            /*
             *     3(g)
             *    /  \
             *   1    8(p)
             *       / \
             *     6(z) 10
             *    / \
             *   5   7
             */

            auto g = std::make_unique<tree_node<int>>(3);
            g->left = std::make_unique<tree_node<int>>(1, g.get());

            auto p = std::make_unique<tree_node<int>>(8, g.get());
            p->right = std::make_unique<tree_node<int>>(10, p.get());

            auto z = std::make_unique<tree_node<int>>(6, p.get());
            z->right = std::make_unique<tree_node<int>>(7, z.get());
            z->left = std::make_unique<tree_node<int>>(5, z.get());

            p->left = std::move(z);
            g->right = std::move(p);

            THEN("a right left rotate balances the tree")
            {
                auto newG = right_left_rotate(std::move(g));

                /*
                 *       6
                 *     /   \
                 *    3     8
                 *   / \   / \
                 *  1   5 7  10
                 */

                REQUIRE(newG->t == 6);
                REQUIRE(newG->left->t == 3);
                REQUIRE(newG->left->left->t == 1);
                REQUIRE(newG->left->right->t == 5);
                REQUIRE(newG->right->t == 8);
                REQUIRE(newG->right->right->t == 10);
                REQUIRE(newG->right->left->t == 7);

                REQUIRE(newG->left->parent == newG.get());
                REQUIRE(newG->right->parent == newG.get());
                REQUIRE(newG->left->left->parent == newG->left.get());
                REQUIRE(newG->left->right->parent == newG->left.get());
                REQUIRE(newG->right->left->parent == newG->right.get());
                REQUIRE(newG->right->right->parent == newG->right.get());
            }
        }

        WHEN("a simple tree has an imbalance in the gradparent's left "
             "child's right subtree")
        {

            /*
             *     5
             *      \
             *      10
             *     /
             *    7
             */

            auto g = std::make_unique<tree_node<int>>(5);

            auto p = std::make_unique<tree_node<int>>(10, g.get());

            auto z = std::make_unique<tree_node<int>>(7, p.get());

            p->left = std::move(z);
            g->right = std::move(p);

            THEN("a right left rotate balances the tree")
            {
                auto newG = right_left_rotate(std::move(g));

                /*
                 *      7
                 *     /  \
                 *    5    10
                 */

                REQUIRE(newG->t == 7);
                REQUIRE(newG->left->t == 5);
                REQUIRE(newG->right->t == 10);

                REQUIRE(newG->left->parent == newG.get());
                REQUIRE(newG->right->parent == newG.get());
            }
        }
    }
} // namespace csb