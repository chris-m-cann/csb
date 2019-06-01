#include "binary_tree/binary_tree.hpp"

#include <catch2/catch.hpp>

namespace csb::test
{

    std::vector<int> make_vector(binary_tree<int> const &bt)
    {
        std::vector<int> v;

        bt.inorder_traverse([&](int i) { v.push_back(i); });

        return v;
    }

    using vector_equals = Catch::Matchers::Vector::EqualsMatcher<int>;

    SCENARIO("traversing")
    {
        GIVEN("a tree")
        {
            auto bt = binary_tree<int>();

            bt.add(3);
            bt.add(1);
            bt.add(5);
            bt.add(8);
            bt.add(2);
            bt.add(22);
            bt.add(15);

            WHEN("traversing in order")
            {
                std::vector<int> v;

                bt.inorder_traverse([&](int i) { v.push_back(i); });

                THEN("nodes are visited in order little to large")
                {
                    REQUIRE(v.size() == 7);

                    auto order = std::vector{1, 2, 3, 5, 8, 15, 22};
                    REQUIRE_THAT(v,
                                 Catch::Matchers::Vector::EqualsMatcher(order));
                }
            }

            WHEN("traversing breadth first")
            {
                std::vector<int> v;

                bt.breadth_first_traverse([&](int i) { v.push_back(i); });

                THEN("nodes are visited in level order left to right")
                {
                    REQUIRE(v.size() == 7);

                    auto order = std::vector{3, 1, 5, 2, 8, 22, 15};
                    REQUIRE_THAT(v,
                                 Catch::Matchers::Vector::EqualsMatcher(order));
                }
            }
        }

        GIVEN("an empty tree")
        {
            auto bt = binary_tree<int>();

            WHEN("traversing in order")
            {
                std::vector<int> v;

                bt.inorder_traverse([&](int i) { v.push_back(i); });

                THEN("no nodes are visited") {}
            }

            WHEN("traversing breadth first")
            {
                std::vector<int> v;

                bt.breadth_first_traverse([&](int i) { v.push_back(i); });

                THEN("no nodes are visited") { REQUIRE(v.empty()); }
            }
        }
    }

    SCENARIO("contains")
    {
        GIVEN("a binary tree")
        {

            auto bt = binary_tree<int>();

            WHEN("it is empty")
            {
                THEN("nothing can be found")
                {
                    REQUIRE_FALSE(bt.contains(3));
                    REQUIRE_FALSE(bt.contains(0));
                    REQUIRE_FALSE(bt.contains(-3));
                    REQUIRE_FALSE(bt.contains(42));
                }
            }

            WHEN("the target has been added")
            {
                bt.add(3);
                bt.add(0);
                bt.add(-3);
                bt.add(42);

                THEN("it is found")
                {
                    REQUIRE(bt.contains(3));
                    REQUIRE(bt.contains(0));
                    REQUIRE(bt.contains(-3));
                    REQUIRE(bt.contains(42));
                }
            }

            WHEN("is not empty but target is not in tree")
            {
                bt.add(3);
                bt.add(0);
                bt.add(-3);
                bt.add(42);

                THEN("it cannot be found")
                {
                    REQUIRE_FALSE(bt.contains(33));
                    REQUIRE_FALSE(bt.contains(10));
                    REQUIRE_FALSE(bt.contains(-33));
                    REQUIRE_FALSE(bt.contains(142));
                }
            }
        }
    }

    SCENARIO("erase")
    {
        GIVEN("a empty tree")
        {
            binary_tree<int> b;

            WHEN("erasing anything")
            {
                b.erase(-1);
                b.erase(0);
                b.erase(34);

                THEN("the tree remains empty") { REQUIRE(b.is_empty()); }
            }
        }

        GIVEN("a non-empty binary-tree")
        {
            binary_tree b{5, -1, 7};

            WHEN("the target is not in the tree")
            {
                b.erase(0);
                b.erase(3);
                THEN("erase does not effect the tree")
                {

                    REQUIRE(b.size() == 3);
                }
            }
        }

        GIVEN("a tree with a single element")
        {
            binary_tree<int> b;
            b.add(9);

            WHEN("removing that element")
            {
                b.erase(9);
                THEN("the tree is empty") { REQUIRE(b.is_empty()); }
            }
        }

        GIVEN("a tree with multiple elements")
        {
            binary_tree b{5, -1, 7, -20, 0, 42, -42, 2, 1, 20, 13, 26, 32};

            /*         5
                      /  \
                    -1    7
                    / \    \
                  -20  0    42
                  /    \     /
                -42     2   20
                       /   /  \
                      1   13  26
                                \
                                32
        */
            WHEN("removing a leaf node")
            {
                b.erase(-42);
                THEN("order of other elements remains unchanged")
                {
                    auto order =
                        std::vector{-20, -1, 0, 1, 2, 5, 7, 13, 20, 26, 32, 42};
                    REQUIRE_THAT(make_vector(b), vector_equals(order));
                    REQUIRE(b.size() == order.size());
                }
            }

            WHEN("removing a node with only a left child which is a leaf")
            {
                b.erase(-20);
                THEN("order of other elements remains unchanged")
                {
                    auto order =
                        std::vector{-42, -1, 0, 1, 2, 5, 7, 13, 20, 26, 32, 42};
                    REQUIRE_THAT(make_vector(b), vector_equals(order));
                    REQUIRE(b.size() == order.size());
                }
            }

            WHEN("removing a node with only a right child which is a leaf")
            {
                b.erase(26);
                THEN("order of other elements remains unchanged")
                {
                    auto order = std::vector{
                        -42, -20, -1, 0, 1, 2, 5, 7, 13, 20, 32, 42};
                    REQUIRE_THAT(make_vector(b), vector_equals(order));
                    REQUIRE(b.size() == order.size());
                }
            }

            WHEN("removing a node with only a left subtree")
            {
                b.erase(42);
                THEN("order of other elements remains unchanged")
                {
                    auto order = std::vector{
                        -42, -20, -1, 0, 1, 2, 5, 7, 13, 20, 26, 32};
                    REQUIRE_THAT(make_vector(b), vector_equals(order));
                    REQUIRE(b.size() == order.size());
                }
            }

            WHEN("removing a node with only a right subtree")
            {
                b.erase(0);
                THEN("order of other elements remains unchanged")
                {
                    auto order = std::vector{
                        -42, -20, -1, 1, 2, 5, 7, 13, 20, 26, 32, 42};
                    REQUIRE_THAT(make_vector(b), vector_equals(order));
                    REQUIRE(b.size() == order.size());
                }
            }

            WHEN("removing a node with both a left and a right subtree")
            {
                b.erase(20);
                THEN("order of other elements remains unchanged")
                {
                    auto order = std::vector{
                        -42, -20, -1, 0, 1, 2, 5, 7, 13, 26, 32, 42};
                    REQUIRE_THAT(make_vector(b), vector_equals(order));
                    REQUIRE(b.size() == order.size());
                }
            }
        }
    }

    SCENARIO("iterator tests")
    {
        GIVEN("an empty binary_tree")
        {
            binary_tree<int> bt;

            THEN("begin == end") { REQUIRE(bt.begin() == bt.end()); }
        }

        GIVEN("a binary_tree with only 1 member")
        {
            binary_tree bt{42};
            THEN("begin points to that member") { REQUIRE(*bt.begin() == 42); }
        }

        GIVEN("a binary_tree with only a right subtree")
        {
            binary_tree bt{1, 2, 3, 4};
            THEN("begin points to the root") { REQUIRE(*bt.begin() == 1); }
        }

        GIVEN("a tree with a left and right subtree")
        {
            binary_tree bt{5, 10, 2, -3, 8, 9, 7};
            /*
                    5
                   / \
                  2   10
                 /    /
               -3    8
                    / \
                   7   9
             */
            THEN("++it from begin -> end traverses the tree in order")
            {
                auto next = bt.begin();
                REQUIRE(*next == -3);
                REQUIRE(*++next == 2);
                REQUIRE(*++next == 5);
                REQUIRE(*++next == 7);
                REQUIRE(*++next == 8);
                REQUIRE(*++next == 9);
                REQUIRE(*++next == 10);
                REQUIRE(++next == bt.end());
            }

            WHEN("using std::copy to build a vector from the iterator range")
            {
                std::vector<int> v;
                std::copy(bt.begin(), bt.end(), std::back_inserter(v));

                THEN("the vector is in order")
                {
                    REQUIRE_THAT(
                        v, vector_equals(std::vector{-3, 2, 5, 7, 8, 9, 10}));
                }
            }
        }
    }

    SCENARIO("copying")
    {
        GIVEN("an empty binary_tree")
        {

            binary_tree<int> b;

            WHEN("copying it to another")
            {

                binary_tree<int> another(b);

                THEN("the other is empty too") { REQUIRE(another.is_empty()); }
            }

            WHEN("assigning it to another")
            {
                binary_tree another{5, 3, 6, 1, 7};

                another = b;
                THEN("the other is empty too") { REQUIRE(another.is_empty()); }
            }
        }

        GIVEN("a populated binary_tree")
        {
            binary_tree b{5, 10, 2, -3, 8, 9, 7};

            /*
                    5
                   / \
                  2   10
                 /    /
               -3    8
                    / \
                   7   9
             */

            WHEN("copying one to another")
            {

                binary_tree another(b);

                THEN("the tree contains nodes in the same locations as the "
                     "previous tree")
                {
                    std::vector<int> v;
                    another.breadth_first_traverse(
                        [&v](int i) { v.push_back(i); });
                    REQUIRE_THAT(
                        v, vector_equals(std::vector{5, 2, 10, -3, 8, 7, 9}));
                }
            }

            WHEN("assigning one to another")
            {
                binary_tree another{6, 8, 3, 5, 9};

                another = b;
                THEN("the tree contains nodes in the same locations as the "
                     "previous tree")
                {
                    std::vector<int> v;
                    another.breadth_first_traverse(
                        [&v](int i) { v.push_back(i); });
                    REQUIRE_THAT(
                        v, vector_equals(std::vector{5, 2, 10, -3, 8, 7, 9}));
                }
            }
        }
    }

    SCENARIO("equality")
    {
        GIVEN("two empty binary_trees")
        {
            binary_tree<int> a;
            binary_tree<int> b;

            WHEN("comparing them")
            {
                THEN("they are equal") { REQUIRE(a == b); }
            }
        }

        GIVEN("one populated binary tree 'a'")
        {
            binary_tree a{6, 8, 3, 5, 9};
            THEN("a == a") { REQUIRE(a == a); }
        }

        GIVEN("two binary_trees 'a' and 'b'")
        {
            binary_tree a{6, 8, 3, 5, 9};
            binary_tree b(a);

            WHEN("a == b")
            {
                REQUIRE(a == b);
                THEN("b == a") { REQUIRE(b == a); }
            }
        }

        GIVEN("3 binary_trees 'a', 'b',and 'c'")
        {
            binary_tree a{6, 8, 3, 5, 9};
            binary_tree b(a);
            binary_tree c(b);
            WHEN("a == b and b == c")
            {
                REQUIRE(a == b);
                REQUIRE(b == c);

                THEN("a == c") { REQUIRE(a == c); }
            }
        }

        GIVEN("two empty binary_trees")
        {
            binary_tree<int> a;
            binary_tree<int> b;

            WHEN(
                "they have the same elements added to them in different orders")
            {
                a.add(5);
                a.add(8);
                a.add(3);
                a.add(1);
                a.add(2);

                b.add(1);
                b.add(2);
                b.add(3);
                b.add(5);
                b.add(8);

                THEN("they are still equal") { REQUIRE(a == b); }
            }
        }
    }

    SCENARIO("find")
    {
        GIVEN("an empty binary_tree")
        {
            binary_tree<int> b;

            THEN("find always returns end")
            {
                REQUIRE(b.find(1) == b.end());
                REQUIRE(b.find(8) == b.end());
                REQUIRE(b.find(-3) == b.end());
                REQUIRE(b.find(0) == b.end());
            }
        }

        GIVEN("a populated binary tree")
        {
            binary_tree b{5, 10, 2, -3, 8, 9, 7};
            /*
                    5
                   / \
                  2   10
                 /    /
               -3    8
                    / \
                   7   9
             */

            WHEN("finding the lowest element")
            {
                auto const found = b.find(-3);

                THEN("begin is returned") { REQUIRE(found == b.begin()); }
            }

            WHEN("finding an element in the middle of the range")
            {
                auto const found = b.find(5);
                THEN("an iterator to that element is returned")
                {
                    REQUIRE(*found == 5);
                }
            }

            WHEN("finding an element that does not exist in the tree")
            {
                THEN("end is returned")
                {
                    REQUIRE(b.find(1) == b.end());
                    REQUIRE(b.find(18) == b.end());
                    REQUIRE(b.find(-33) == b.end());
                    REQUIRE(b.find(0) == b.end());
                }
            }
        }
    }

    SCENARIO("traversing backwards")
    {
        GIVEN("a populated binary tree")
        {
            binary_tree b{5, 10, 2, -3, 8, 9, 7};

            /*
                    5
                   / \
                  2   10
                 /    /
               -3    8
                    / \
                   7   9
             */

            WHEN("decrementing end")
            {
                auto const decd = --b.end();

                THEN("an iterator to the largest element is returned")
                {
                    REQUIRE(*decd == 10);
                }
            }

            WHEN("decrementing from end")
            {
                auto it = b.end();
                THEN("the tree is traversed from largest to smallest ending "
                     "wth begin")
                {
                    REQUIRE(*--it == 10);
                    REQUIRE(*--it == 9);
                    REQUIRE(*--it == 8);
                    REQUIRE(*--it == 7);
                    REQUIRE(*--it == 5);
                    REQUIRE(*--it == 2);
                    REQUIRE(*--it == -3);
                    REQUIRE(it == b.begin());
                }
            }
        }
    }
} // namespace csb::test
