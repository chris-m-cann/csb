#include <cstddef>
#include <utility>

#include <catch2/catch.hpp>

#include "static_array.hpp"
#include <core/type_traits.hpp>

namespace csb::test
{
    namespace
    {
        template <typename... Args> auto create_sut(Args &&... args)
        {
            return csb::static_array<int, sizeof...(Args)>{
                std::forward<Args>(args)...};
        }
    } // namespace

    SCENARIO("static_array equality")
    {
        // reflexive
        GIVEN("a static_array a")
        {
            auto a = create_sut(1, 2, 3, 4);

            THEN("a == a") { REQUIRE(a == a); }
        }

        GIVEN("2 static_arrays, a and b")
        {
            auto a = create_sut(1, 2, 3, 4);
            auto b = create_sut(1, 2, 3, 4);

            WHEN("a == b")
            {
                REQUIRE(a == b);

                THEN("b == a") { REQUIRE(b == a); }
            }
        }

        GIVEN("3 static_arrays, a, b and c")
        {
            auto a = create_sut(1, 2, 3, 4);
            auto b = create_sut(1, 2, 3, 4);
            auto c = create_sut(1, 2, 3, 4);

            WHEN("a == b and b == c")
            {
                REQUIRE(a == b);
                REQUIRE(b == c);

                THEN("a == c") { REQUIRE(a == c); }
            }
        }

        GIVEN("2 static_arrays containing the same elements")
        {
            auto a = create_sut(1, 2, 3, 4);

            WHEN("those elements are in the same order")
            {
                auto b = create_sut(1, 2, 3, 4);
                THEN("the 2 are equal")
                {
                    REQUIRE(a == b);
                    REQUIRE_FALSE(a != b);
                }
            }

            WHEN("those elements are in different orders")
            {
                auto b = create_sut(4, 3, 2, 1);
                THEN("the 2 are not equal")
                {
                    REQUIRE(a != b);
                    REQUIRE_FALSE(a == b);
                }
            }
        }
    }

    SCENARIO("static_array copyable")
    {
        GIVEN("an array a")
        {
            auto a = create_sut(1, 2, 3, 4, 5);
            WHEN("copy constructing b from a")
            {
                decltype(a) b(a);

                THEN("a == b") { REQUIRE(a == b); }

                WHEN("altering b")
                {
                    b[1]++;
                    THEN("a != b") { REQUIRE(a != b); }
                }

                WHEN("altering a")
                {
                    a[1]++;
                    THEN("a != b") { REQUIRE(a != b); }
                }
            }

            WHEN("copy assigning a to b")
            {
                auto b = create_sut(9, 8, 7, 6, 5);
                b = a;

                THEN("a == b") { REQUIRE(a == b); }

                WHEN("altering b")
                {
                    b[1]++;
                    THEN("a != b") { REQUIRE(a != b); }
                }

                WHEN("altering a")
                {
                    a[1]++;
                    THEN("a != b") { REQUIRE(a != b); }
                }
            }
        }
    }

    SCENARIO("static_array movable")
    {
        GIVEN("2 equal static_arrays a and b")
        {
            auto a = create_sut(1, 3, 5, 7, 9);
            auto b = create_sut(1, 3, 5, 7, 9);

            WHEN("move constructing a third array c from b")
            {
                decltype(a) c(std::move(b));
                THEN("c == a") { REQUIRE(c == a); }
            }

            WHEN("move assigning to a third array c from b")
            {
                decltype(a) c = create_sut(5, 6, 7, 8, 9);

                c = std::move(b);

                THEN("c == a") { REQUIRE(c == a); }
            }
        }
    }

    SCENARIO("static_array Range init")
    {
        GIVEN("A Range of elements a, b, c")
        {
            auto a = 1;
            auto b = 2;
            auto c = 3;
            int abc[] = {a, b, c};

            WHEN("Initializing a static_array of size 3")
            {
                static_array<int, 3> sut(abc);

                THEN("Array has element a, b and c in order")
                {
                    REQUIRE(sut[0] == a);
                    REQUIRE(sut[1] == b);
                    REQUIRE(sut[2] == c);
                }
            }
            WHEN("Initializing a static_array of size 2")
            {
                static_array<int, 2> sut(abc);
                THEN("Array has element a, b in order")
                {
                    REQUIRE(sut[0] == a);
                    REQUIRE(sut[1] == b);
                }
            }
            WHEN("Initializing a static_array of size 4")
            {
                static_array<int, 4> sut(abc);
                THEN("Array has element a, b and c in order")
                {
                    REQUIRE(sut[0] == a);
                    REQUIRE(sut[1] == b);
                    REQUIRE(sut[2] == c);
                }
            }
        }
    }

    SCENARIO("static_array operator[]")
    {
        constexpr csb::static_array<int, 3> arr{1, 2, 3};
        constexpr int a = arr[1];
        static_assert(a == 2, "grr");
    }

    SCENARIO("statuc_array::at")
    {
        constexpr csb::static_array<int, 3> arr{1, 2, 3};
        constexpr int a = arr.at(1);
        static_assert(a == 2, "grr");
    }

    SCENARIO("static_array fill ctor")
    {
        GIVEN("a static_array of size 3")
        {
            WHEN("when initializing it with value_type v")
            {
                int v = 4;
                csb::static_array<int, 3> sut(v);

                THEN("sll three elements are of value v")
                {
                    REQUIRE(sut[0] == v);
                    REQUIRE(sut[1] == v);
                    REQUIRE(sut[2] == v);
                }
            }
        }
    }
} // namespace csb::test