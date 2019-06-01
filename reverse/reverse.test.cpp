#include "reverse/reverse.hpp"

#include <catch2/catch.hpp>

#include <cstring>
#include <list>

namespace csb::test
{
    SCENARIO("reversing a string")
    {
        GIVEN("an empty string")
        {
            char empty[] = "";

            WHEN("reversing")
            {

                reverse(empty);

                THEN("the string is still empty")
                {
                    REQUIRE(std::strlen(empty) == 0);
                }
            }
        }

        GIVEN("an even sized string")
        {
            char even[] = "four";

            WHEN("reversing")
            {

                reverse(even);

                THEN("the string is reversed")
                {
                    REQUIRE(std::strlen(even) == 4);
                    REQUIRE(std::strcmp(even, "ruof") == 0);
                }
            }
        }

        GIVEN("an odd sized string")
        {
            char odd[] = "some odd length";

            WHEN("reversing")
            {

                reverse(odd);

                THEN("the string is reversed")
                {
                    REQUIRE(std::strlen(odd) == 15);
                    REQUIRE(std::strcmp(odd, "htgnel ddo emos") == 0);
                }
            }
        }
    }

    SCENARIO("reversing an array")
    {

        GIVEN("an even sized array")
        {
            int even[] = {1, 2, 3, 4};

            WHEN("reversing")
            {

                reverse(std::begin(even), std::end(even));

                THEN("the array is reversed")
                {
                    REQUIRE(sizeof(even) == 4 * sizeof(int));
                    auto const reversed = {4, 3, 2, 1};
                    REQUIRE(std::equal(std::begin(reversed),
                                       std::end(reversed),
                                       std::begin(even)));
                }
            }
        }

        GIVEN("an od sized array")
        {
            int odd[] = {1, 2, 3, 4, 5};

            WHEN("reversing")
            {

                reverse(std::begin(odd), std::end(odd));

                THEN("the array is reversed")
                {
                    REQUIRE(sizeof(odd) == 5 * sizeof(int));
                    auto const reversed = {5, 4, 3, 2, 1};
                    REQUIRE(std::equal(std::begin(reversed),
                                       std::end(reversed),
                                       std::begin(odd)));
                }
            }
        }
    }

    SCENARIO("reversing a doubly linked list")
    {
        GIVEN("an empty list")
        {
            std::list<int> empty;

            WHEN("reversing")
            {

                csb::reverse(std::begin(empty), std::end(empty));

                THEN("the array is still empty") { REQUIRE(empty.empty()); }
            }
        }

        GIVEN("an even sized list")
        {
            std::list<int> even{1, 2, 3, 4};

            WHEN("reversing")
            {

                csb::reverse(std::begin(even), std::end(even));

                THEN("the array is reversed")
                {
                    REQUIRE(even.size() == 4);
                    auto const reversed = {4, 3, 2, 1};
                    REQUIRE(std::equal(std::begin(reversed),
                                       std::end(reversed),
                                       std::begin(even)));
                }
            }
        }

        GIVEN("an od sized list")
        {
            std::list<int> odd{1, 2, 3, 4, 5};

            WHEN("reversing")
            {

                csb::reverse(std::begin(odd), std::end(odd));

                THEN("the array is reversed")
                {
                    REQUIRE(odd.size() == 5);
                    auto const reversed = {5, 4, 3, 2, 1};
                    REQUIRE(std::equal(std::begin(reversed),
                                       std::end(reversed),
                                       std::begin(odd)));
                }
            }
        }
    }
} // namespace csb::test