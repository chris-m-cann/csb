#include "red_black_tree.hpp"

#include <algorithm>
#include <catch2/catch.hpp>
#include <iostream>

namespace csb
{
    namespace
    {
        using node_type = red_black_tree<int>::node_type;

        constexpr auto red() { return impl::Colour::Red; }
        constexpr auto black() { return impl::Colour::Black; }

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

        std::vector<int> values(std::vector<node_type const *> const &nodes)
        {
            std::vector<int> vs;
            std::transform(nodes.begin(),
                           nodes.end(),
                           std::back_inserter(vs),
                           [](auto const &n) { return n->t; });
            return vs;
        }

        auto colours(std::vector<node_type const *> const &nodes)
        {
            std::vector<impl::Colour> cs;
            std::transform(nodes.begin(),
                           nodes.end(),
                           std::back_inserter(cs),
                           [](auto const &n) { return n->metadata().colour; });
            return cs;
        }

        auto make_node(int v, impl::Colour colour, node_type *parent = nullptr)
        {
            auto node = std::make_unique<node_type>(std::move(v), parent);
            node->metadata().colour = colour;
            return node;
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

    SCENARIO("Red black tree Fuzz insertions")
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
                    // gen into an int in [1, 100]
                    //                    std::cout << gend << ' ';
                    rb.add(gend);
                }
                //                std::cout << std::endl;

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

    SCENARIO("Redblack tree Deletion")
    {
        GIVEN("a tree with a single node")
        {
            red_black_tree<int> rb;
            rb.add(42);

            WHEN("Deleting that node")
            {
                rb.erase(42);

                THEN("The tree is empty") { REQUIRE(rb.is_empty()); }
            }
        }

        GIVEN("node to delete is red")
        {

            auto root = make_node(30, black());
            auto black40 = make_node(40, black(), root.get());
            auto black20 = make_node(20, black(), root.get());
            auto red10 = make_node(10, red(), black20.get());

            black20->left = std::move(red10);
            root->left = std::move(black20);
            root->right = std::move(black40);

            red_black_tree<int> rb(std::move(root));

            WHEN("removing it")
            {
                rb.erase(10);

                THEN("The remaining tree positions and colours are unchanged")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{30, 20, 40}));

                    REQUIRE_THAT(colours(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector(3, impl::Colour::Black)));
                }
            }
        }

        GIVEN("node to delete is red and it's child is black")
        {
            auto root = make_node(30, black());
            auto black40 = make_node(40, black(), root.get());
            auto red20 = make_node(20, red(), root.get());
            auto black10 = make_node(10, black(), red20.get());

            red20->left = std::move(black10);
            root->left = std::move(red20);
            root->right = std::move(black40);

            red_black_tree<int> rb(std::move(root));

            WHEN("removing it")
            {
                rb.erase(20);

                THEN("The remaining tree positions and colours are unchanged")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{30, 10, 40}));

                    REQUIRE_THAT(colours(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector(3, impl::Colour::Black)));
                }
            }
        }

        GIVEN("node to delete v is black, parent's right child s is black and "
              "s's right "
              "child, r, is red")
        {
            auto root = make_node(30, black());
            auto s = make_node(40, black(), root.get());
            auto v = make_node(20, black(), root.get());
            auto r = make_node(50, red(), s.get());

            s->right = std::move(r);
            root->right = std::move(s);
            root->left = std::move(v);

            red_black_tree<int> rb(std::move(root));

            WHEN("removing it")
            {
                rb.erase(20);

                THEN("The remaining nodes have been rotated")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{40, 30, 50}));

                    REQUIRE_THAT(colours(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{impl::Colour::Black,
                                                 impl::Colour::Black,
                                                 impl::Colour::Black}));
                }
            }
        }

        GIVEN("node to delete v is black, parent's left child s is black and "
              "s's left"
              " child,r, is red")
        {

            auto root = make_node(30, black());
            auto s = make_node(20, black(), root.get());
            auto v = make_node(40, black(), root.get());
            auto r = make_node(10, red(), s.get());
            auto otherRed = make_node(25, red(), s.get());

            s->left = std::move(r);
            s->right = std::move(otherRed);
            root->left = std::move(s);
            root->right = std::move(v);

            red_black_tree<int> rb(std::move(root));

            WHEN("removing it")
            {
                rb.erase(40);

                THEN("The remaining nodes have been rotated")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{20, 10, 30, 25}));

                    REQUIRE_THAT(colours(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{impl::Colour::Black,
                                                 impl::Colour::Black,
                                                 impl::Colour::Black,
                                                 impl::Colour::Red}));
                }
            }
        }

        GIVEN("node to delete v is black, parent's right child s is black and "
              "s's left"
              " child,r, is red")
        {

            auto root = make_node(30, black());
            auto s = make_node(40, black(), root.get());
            auto v = make_node(20, black(), root.get());
            auto r = make_node(35, red(), s.get());

            s->left = std::move(r);
            root->left = std::move(v);
            root->right = std::move(s);

            red_black_tree<int> rb(std::move(root));

            WHEN("removing it")
            {
                rb.erase(20);

                THEN("The remaining nodes have been rotated")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{35, 30, 40}));

                    REQUIRE_THAT(colours(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{impl::Colour::Black,
                                                 impl::Colour::Black,
                                                 impl::Colour::Black}));
                }
            }
        }

        GIVEN("node to delete v is black, parent's left child s is black and "
              "s's right"
              " child,r, is red")
        {

            auto root = make_node(30, black());
            auto s = make_node(20, black(), root.get());
            auto v = make_node(40, black(), root.get());
            auto r = make_node(25, red(), s.get());

            s->right = std::move(r);
            root->left = std::move(s);
            root->right = std::move(v);

            red_black_tree<int> rb(std::move(root));

            WHEN("removing it")
            {
                rb.erase(40);

                THEN("The remaining nodes have been rotated")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{25, 20, 30}));

                    REQUIRE_THAT(colours(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{impl::Colour::Black,
                                                 impl::Colour::Black,
                                                 impl::Colour::Black}));
                }
            }
        }

        GIVEN(
            "v is black, p is red, s is black and both s's children are black")
        {
            auto root = make_node(50, black());
            auto p = make_node(20, red(), root.get());
            auto v = make_node(10, black(), p.get());
            auto s = make_node(30, black(), p.get());
            auto rightOfRoot = make_node(70, black(), root.get());
            auto rightRight = make_node(80, red(), rightOfRoot.get());

            p->right = std::move(s);
            p->left = std::move(v);
            rightOfRoot->right = std::move(rightRight);
            root->right = std::move(rightOfRoot);
            root->left = std::move(p);

            red_black_tree<int> rb(std::move(root));

            WHEN("deleting v")
            {
                rb.erase(10);

                THEN("s and p are recolured")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{50, 20, 70, 30, 80}));

                    REQUIRE_THAT(
                        colours(levels),
                        Catch::Matchers::Vector::EqualsMatcher(std::vector{
                            black(), black(), black(), red(), red()}));
                }
            }
        }

        GIVEN("v is black, p is black, s is black and both s's children are "
              "black")
        {
            auto root = make_node(50, black());
            auto p = make_node(20, black(), root.get());
            auto v = make_node(10, black(), p.get());
            auto s = make_node(30, black(), p.get());
            auto rightOfRoot = make_node(70, black(), root.get());
            auto rightRight = make_node(80, red(), rightOfRoot.get());
            auto rightRightRight = make_node(90, black(), rightRight.get());

            p->right = std::move(s);
            p->left = std::move(v);
            root->left = std::move(p);

            rightRight->right = std::move(rightRightRight);
            rightOfRoot->right = std::move(rightRight);
            root->right = std::move(rightOfRoot);

            red_black_tree<int> rb(std::move(root));

            WHEN("deleting v")
            {
                rb.erase(10);
                THEN("s is recoloured and tree is rearranged based on the "
                     "placement of p")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{70, 50, 80, 20, 90, 30}));

                    REQUIRE_THAT(colours(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{black(),
                                                 black(),
                                                 black(),
                                                 black(),
                                                 black(),
                                                 red()}));
                }
            }
        }

        GIVEN("v is black, p is black, s is red, s = p->right")
        {
            auto p = make_node(20, black());
            auto v = make_node(10, black(), p.get());
            auto s = make_node(30, red(), p.get());
            auto lc = make_node(25, black(), s.get());
            auto rc = make_node(35, black(), s.get());

            s->left = std::move(lc);
            s->right = std::move(rc);
            p->right = std::move(s);
            p->left = std::move(v);

            red_black_tree<int> rb(std::move(p));

            WHEN("erasing v")
            {
                rb.erase(10);

                THEN("tree is left rotated about s, s and p are recoloured and "
                     "tree is balanced via other cases")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{30, 20, 35, 25}));

                    REQUIRE_THAT(
                        colours(levels),
                        Catch::Matchers::Vector::EqualsMatcher(
                            std::vector{black(), black(), black(), red()}));
                }
            }
        }

        GIVEN("v is black, p is black, s is red, s = p->left")
        {
            auto p = make_node(40, black());
            auto v = make_node(50, black(), p.get());
            auto s = make_node(30, red(), p.get());
            auto lc = make_node(25, black(), s.get());
            auto rc = make_node(35, black(), s.get());

            s->left = std::move(lc);
            s->right = std::move(rc);
            p->left = std::move(s);
            p->right = std::move(v);

            red_black_tree<int> rb(std::move(p));

            WHEN("erasing v")
            {
                rb.erase(50);

                THEN("tree is right rotated about s, s and p are recoloured "
                     "and tree is balanced via other cases")
                {
                    auto levels = level_order(rb);
                    REQUIRE_THAT(values(levels),
                                 Catch::Matchers::Vector::EqualsMatcher(
                                     std::vector{30, 25, 40, 35}));

                    REQUIRE_THAT(
                        colours(levels),
                        Catch::Matchers::Vector::EqualsMatcher(
                            std::vector{black(), black(), black(), red()}));
                }
            }
        }
    }

    SCENARIO("Red black Fuzz deletions")
    {
        using impl::Colour;

        GIVEN("a red black tree with a random list of integers")
        {
            red_black_tree<int> rb;

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 100);
            std::vector<int> values;

            auto n = dis(gen) + 1;
            for (int i = 0; i != n; ++i)
            {
                auto gend = dis(gen);
                values.push_back(gend);
                rb.add(gend);
            }

            WHEN("deleting random nodes")
            {
                std::random_device rd2;
                std::mt19937 gen2(rd());

                while (rb.size() > 1)
                {
                    std::uniform_int_distribution<> dis2(0, values.size() - 1);

                    auto idx = dis2(gen2);
                    auto target = values[idx];
                    //                    std::cerr << "values.size = " <<
                    //                    values.size() << " idx = " << idx << "
                    //                    target = " << target << std::endl;
                    values.erase(values.begin() + idx);

                    auto backup = rb;

                    rb.erase(target);

                    auto v = level_order(rb);
                    auto root = *v.begin();

                    THEN("the root is black")
                    {
                        REQUIRE(root->metadata().colour == Colour::Black);
                    }

                    THEN("there are no adjacent red nodes")
                    {
                        auto is_black =
                            [](red_black_tree<int>::node_type const *n)
                            -> bool {
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

                    THEN("Distance to furthest leaf is <= 2 * distance to "
                         "closest "
                         "leaf")
                    {
                        REQUIRE(max_height(root) <= 2 * min_height(root));
                    }

                    THEN("Every route from root to leaf has the same number of "
                         "black nodes")
                    {
                        auto height = compute_black_height(root);

                        REQUIRE(height >= 0);
                    }
                }
            }
        }
    }

} // namespace csb

// i know this is technically undefined behaviour but it works and makes thigs
// sooo much easier to debug
namespace std
{
    template <typename T, typename V>
    ostream &operator<<(ostream &os, pair<T, V> const &p)
    {
        return os << '(' << p.first << ',' << p.second << ')';
    }
} // namespace std
