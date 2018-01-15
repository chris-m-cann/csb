#pragma once

#include "test_util.hpp"
#include "concepts.hpp"

#include <catch/catch.hpp>

namespace csb
{

    template <ccept::Regular Sut, ccept::Invokable<void(Sut &)> Fn>
    auto copyable(Sut a, Fn modify, std::string s)
    {
        GIVEN_V("a list, a : ", a, " | context =", s)
        {
            WHEN("copy constructing a new list (b) from a")
            {
                Sut b(a);

                THEN("a == b") { REQUIRE(a == b); }

                WHEN("altering b")
                {
                    modify(b);
                    THEN("a != b") { REQUIRE(a != b); }
                }

                WHEN("altering a")
                {
                    modify(a);
                    THEN("a != b") { REQUIRE(a != b); }
                }
            }
        }

        GIVEN_V("two lists a and b where a=", a, "|(", s, ")")
        {
            Sut b;
            WHEN("copy assignin a to b")
            {
                b = a;

                THEN("a == b") { REQUIRE(a == b); }
                WHEN("altering a")
                {
                    modify(a);
                    THEN("a != b") { REQUIRE(a != b); }
                }
                WHEN("altering b")
                {
                    modify(b);
                    THEN("a != b") { REQUIRE(a != b); }
                }
            }
        }
    }

    template <ccept::Regular Sut> auto moveable(Sut a, Sut expected)
    {
        GIVEN_V("b is movecontructed from a, where a=", a)
        {
            Sut b(std::move(a));
            THEN("b == expected") { REQUIRE(b == expected); }
        }
        GIVEN_V("b is move_assigned the value from a, where a=", a)
        {
            Sut b;
            b = std::move(a);
            THEN("b has taken on the value of a") { REQUIRE(b == expected); }
        }
    }

    template <ccept::EqualityComparable T> auto equality_reflexive(T a)
    {
        GIVEN_V("one list a=", a)
        {
            THEN("a == a") { REQUIRE(a == a); }
        }
    }

    template <ccept::EqualityComparable T> auto equality_symetric(T a, T b)
    {
        GIVEN_V("two lists a and b \n(a=", a, "),\n(b=", b, ")")
        {
            WHEN("a == b")
            {
                REQUIRE(a == b);

                THEN("b == a") { REQUIRE(b == a); }
            }
        }
    }

    template <ccept::EqualityComparable T>
    auto equality_transitive(T a, T b, T c)
    {
        GIVEN_V(
            "three lists a, b and c \n(a=", a, "),\n(b=", b, ")\n(c=", c, ")")
        {
            WHEN("a == b and b == c")
            {
                REQUIRE(a == b);
                REQUIRE(b == c);

                THEN("a == c") { REQUIRE(a == c); }
            }
        }
    }
}
