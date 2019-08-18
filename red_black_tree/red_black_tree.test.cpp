#include "red_black_tree.hpp"

#include <algorithm>
#include <catch2/catch.hpp>
#include <iostream>

namespace csb
{
    namespace
    {

        template <class Container, typename... Args>
        void insert(Container &container, Args &&... args)
        {
            (container.add(std::forward<Args>(args)), ...);
        }

        template <typename T>
        std::vector<typename red_black_tree<T>::node_type const *>
        level_order(red_black_tree<T> const &rb)
        {

            std::vector<typename red_black_tree<T>::node_type const *> v;

            rb.breadth_first_traverse_nodes(
                [&](typename red_black_tree<T>::node_type const &n) {
                    v.push_back(&n);
                });
            return v;
        }

        auto compute_black_height(red_black_tree<int>::node_type const *n)
            -> int
        {
            if (n == nullptr)
                return 0;

            int left_height = compute_black_height(n->left.get());
            int right_height = compute_black_height(n->right.get());

            /*
             * current subtree not a red back tree if left_height !=
             * right_height also if either subtree detects a fault they return
             * height -1
             */
            if (left_height != right_height || left_height == -1)
            {
                return -1;
            }
            else
            {
                // add to height if the colour of the node is black
                return left_height +
                       (n->metadata().colour == impl::Colour::Black ? 1 : 0);
            }
        }

        using pick_fn_t = int const &(int const &, int const &);

        int tree_height(red_black_tree<int>::node_type const *n, pick_fn_t pick)
        {
            if (n == nullptr)
                return 0;

            if (n->left == nullptr && n->right == nullptr)
            {
                return 1;
            }

            if (n->left == nullptr)
            {
                return tree_height(n->right.get(), pick);
            }

            if (n->right == nullptr)
            {
                return tree_height(n->left.get(), pick);
            }

            return pick(tree_height(n->left.get(), pick),
                        tree_height(n->right.get(), pick)) +
                   1;
        }
        int min_height(red_black_tree<int>::node_type const *n)
        {
            return tree_height(n, std::min);
        }

        int max_height(red_black_tree<int>::node_type const *n)
        {
            return tree_height(n, std::max);
        }
    } // namespace
    SCENARIO("insert at root")
    {
        GIVEN("an empty red black tree")
        {

            red_black_tree<int> rb;

            WHEN("inserting the first element")
            {

                rb.add(42);

                THEN("it is black")
                {
                    REQUIRE(*rb.begin() == 42);
                    REQUIRE(rb.begin().node().metadata().colour ==
                            impl::Colour::Black);
                }
            }
        }
    }

    SCENARIO("recolouring")
    {
        WHEN("new node's parent and aunt are red")
        {
            /*      10b
                   /    \
                  5r    15r
                          \
                          20r
            */

            red_black_tree<int> rb;
            rb.add(10);
            rb.add(5);
            rb.add(15);

            // new node
            rb.add(20);
            THEN("recolour tree")
            {
                /*      10b
                       /    \
                      5b    15b
                              \
                              20r
                */

                auto v = level_order(rb);

                auto it = v.begin();
                REQUIRE((*it)->t == 10);
                REQUIRE((*it)->metadata().colour == impl::Colour::Black);

                REQUIRE((*++it)->t == 5);
                REQUIRE((*it)->metadata().colour == impl::Colour::Black);

                REQUIRE((*++it)->t == 15);
                REQUIRE((*it)->metadata().colour == impl::Colour::Black);

                REQUIRE((*++it)->t == 20);
                REQUIRE((*it)->metadata().colour == impl::Colour::Red);
            }
        }
    }

    SCENARIO("Rotation insertion balancing")
    {
        GIVEN("A Balanced red black tree")
        {
            red_black_tree<int> rb;

            WHEN("the next insertion causes an imbalance in roots left childs "
                 "left subtree")
            {
                insert(rb, 10, 5);

                /*
                 *     10(b)
                 *    /   \
                 *   5(r)  null(b)
                 */
                rb.add(2);

                THEN("tree is balanced")
                {
                    using impl::Colour;
                    std::vector expected{std::pair(5, Colour::Black),
                                         std::pair(2, Colour::Red),
                                         std::pair(10, Colour::Red)};

                    auto nodes = level_order(rb);
                    decltype(expected) actual;
                    std::transform(std::begin(nodes),
                                   std::end(nodes),
                                   std::back_inserter(actual),
                                   [](auto np) {
                                       return std::pair(np->t,
                                                        np->metadata().colour);
                                   });

                    REQUIRE_THAT(
                        actual,
                        Catch::Matchers::Vector::EqualsMatcher(expected));
                }
            }

            WHEN("next insertion causes an imbalance is in the roots left "
                 "child's right subtree")
            {
                insert(rb, 10, 5);

                /*
                 *      10(b)
                 *     /   \
                 *    5(r)  null(b)
                 */
                rb.add(7);

                THEN("balance the tree")
                {
                    using impl::Colour;
                    std::vector expected{std::pair(7, Colour::Black),
                                         std::pair(5, Colour::Red),
                                         std::pair(10, Colour::Red)};

                    auto nodes = level_order(rb);
                    decltype(expected) actual;
                    std::transform(std::begin(nodes),
                                   std::end(nodes),
                                   std::back_inserter(actual),
                                   [](auto np) {
                                       return std::pair(np->t,
                                                        np->metadata().colour);
                                   });

                    REQUIRE_THAT(
                        actual,
                        Catch::Matchers::Vector::EqualsMatcher(expected));
                }
            }

            WHEN("next insertion causes an imbalance is in the roots right "
                 "child's right subtree")
            {
                insert(rb, 10, 12);

                /*
                 *      10(b)
                 *     /    \
                 *  null(b)  12(r)
                 */
                rb.add(15);

                THEN("balance the tree")
                {
                    using impl::Colour;
                    std::vector expected{std::pair(12, Colour::Black),
                                         std::pair(10, Colour::Red),
                                         std::pair(15, Colour::Red)};

                    auto nodes = level_order(rb);
                    decltype(expected) actual;
                    std::transform(std::begin(nodes),
                                   std::end(nodes),
                                   std::back_inserter(actual),
                                   [](auto np) {
                                       return std::pair(np->t,
                                                        np->metadata().colour);
                                   });

                    REQUIRE_THAT(
                        actual,
                        Catch::Matchers::Vector::EqualsMatcher(expected));
                }
            }

            WHEN("next insertion causes an imbalance is in the roots right "
                 "child's left subtree")
            {
                insert(rb, 10, 12);

                /*
                 *      10(b)
                 *     /    \
                 *  null(b)  12(r)
                 */
                rb.add(11);

                THEN("balance the tree")
                {
                    using impl::Colour;
                    std::vector expected{std::pair(11, Colour::Black),
                                         std::pair(10, Colour::Red),
                                         std::pair(12, Colour::Red)};

                    auto nodes = level_order(rb);
                    decltype(expected) actual;
                    std::transform(std::begin(nodes),
                                   std::end(nodes),
                                   std::back_inserter(actual),
                                   [](auto np) {
                                       return std::pair(np->t,
                                                        np->metadata().colour);
                                   });

                    REQUIRE_THAT(
                        actual,
                        Catch::Matchers::Vector::EqualsMatcher(expected));
                }
            }
        }
    }

    SCENARIO("Fuzz insertions")
    {
        using impl::Colour;

        GIVEN("a n empty red black tree")
        {
            red_black_tree<int> rb;

            WHEN("inserting a random list of integers")
            {

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(1, 100);

                auto n = dis(gen) + 1;
                for (int i = 0; i != n; ++i)
                {
                    auto gend = dis(gen);
                    // Use dis to transform the random unsigned int generated by
                    // gen into an int in [1, 6]
                    std::cout << gend << ' ';
                    rb.add(gend);
                }
                std::cout << std::endl;

                auto v = level_order(rb);
                auto root = *v.begin();

                THEN("the root is black")
                {
                    REQUIRE(root->metadata().colour == Colour::Black);
                }

                THEN("there are no adjacent red nodes")
                {
                    auto is_black =
                        [](red_black_tree<int>::node_type const *n) -> bool {
                        return n == nullptr ||
                               n->metadata().colour == Colour::Black;
                    };

                    for (auto it = rb.begin(); it != rb.end(); ++it)
                    {
                        if (it.node().metadata().colour == Colour::Red)
                        {
                            REQUIRE(is_black(it.node().left.get()));
                            REQUIRE(is_black(it.node().right.get()));
                        }
                    }
                }

                THEN("Distance to furthest leaf is <= 2 * distance to closest "
                     "leaf")
                {
                    REQUIRE(max_height(root) <= 2 * min_height(root));
                }

                THEN("Every route from root to leaf has the same number of "
                     "black nodes")
                {
                    REQUIRE(compute_black_height(root) >= 0);
                }
            }
        }
    }
} // namespace csb

namespace std
{
    template <typename T, typename V>
    ostream &operator<<(ostream &os, pair<T, V> const &p)
    {
        return os << '(' << p.first << ',' << p.second << ')';
    }
} // namespace std

/*
 * next things to do
 * - unify the impl::binary_tree_node and the tree_node
 * - implement the rotate insertion scenarios
 *  - clean it up!!
 *
 */