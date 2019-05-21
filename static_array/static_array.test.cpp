#include "static_array.hpp"

#include <core/is_regular_test.hpp>
#include <core/test_util.hpp>

#include <catch/catch.hpp>

namespace test
{
    namespace
    {
        template <int i = 3> struct int_generator
        {
            using sut_t = csb::static_array<int, i>;
            using data_t = int;

            sut_t create_sut() const { return {}; }
            int_data_generator data_source() const { return {}; }
        };
        template <int i = 3> struct custom_generator
        {
            using sut_t = csb::static_array<custom, i>;
            using data_t = custom;

            sut_t create_sut() const { return {}; }
            custom_data_generator data_source() const { return {}; }
        };
    } // namespace

    template <int size> struct arg_printer<csb::static_array<int, size>>
    {
        static std::ostream &print(std::ostream &os,
                                   csb::static_array<int, size> const &sa)
        {
            os << "static_array<int>[";
            for (auto const &i : sa)
            {
                os << i << ",";
            }
            os << (sa.is_empty() ? "]" : "\b]");
            return os;
        }
    };

    template <int size> struct arg_printer<csb::static_array<custom, size>>
    {
        static std::ostream &print(std::ostream &os,
                                   csb::static_array<custom, size> const &sa)
        {
            os << "static_array<custom>[";
            for (auto const &i : sa)
            {
                os << i << ",";
            }
            os << (sa.is_empty() ? "]" : "\b]");
            return os;
        }
    };

    template <std::size_t size, typename... Args>
    auto create_sut(Args &&... args)
    {
        return csb::static_array<int, size>{std::forward<Args>(args)...};
    }

    auto data_generator() { return int_data_generator(); }

    template <SutGenerator G> auto equality(G g)
    {
        // empty list is reflexive and equal to itself and transitive
        auto empty = g.create_sut();
        equality_reflexive(empty);
        auto also_empty = g.create_sut();
        equality_symetric(empty, also_empty);
        auto empty_as_well = g.create_sut();
        equality_transitive(empty, empty_as_well, also_empty);

        // two lists with the same number of elements in the same sequence
        auto not_empty = g.create_sut();
        auto also_not_empty = g.create_sut();
        auto not_empty_as_well = g.create_sut();
        auto gen = g.data_source();
        data_t<G> in[] = {gen(), gen(), gen()};

        not_empty.assign(in);
        also_not_empty.assign(in);
        not_empty_as_well.assign(in);

        // different values are !=
        // different orders are !=
        GIVEN_T("two lists with the same size", G)
        {
            auto a = g.create_sut();
            auto b = g.create_sut();

            auto gen = g.data_source();
            auto v1 = gen();
            auto v2 = gen();
            auto v3 = gen();

            WHEN("the values contained are different")
            {
                a.fill(v1);
                b.fill(v2);
                THEN("they are not equal") { REQUIRE(a != b); }
            }
            WHEN("with equal nodes in different orders")
            {
                auto order1 = {v1, v2, v3};
                auto order2 = {v2, v1, v3};
                a.assign(order1);
                b.assign(order2);
                THEN("they are not equal") { REQUIRE(a != b); }
            }
        }
    }

    SCENARIO("static_array: equality")
    {
        equality(int_generator<3>{});
        equality(int_generator<1>{});

        equality(custom_generator<1>{});
        equality(custom_generator<5>{});
    }

    SCENARIO("copyable", "static_array")
    {
        csb::static_array<int, 3> i{1, 2, 3};
        copyable(i, [](auto &array) { ++array.front(); }, "modify front");

        custom_data_generator gen;
        csb::static_array<custom, 3> c(gen());
        copyable(c,
                 [&](auto &array) { array.front() = gen(); },
                 "modify front of custom array");
    }

    SCENARIO("moveable")
    {
        csb::static_array<int, 1> i{1};
        csb::static_array<int, 1> expected{1};
        moveable(i, expected);

        expected.front() = i.front() = 42;
        moveable(i, expected);

        csb::static_array<custom, 2> c;
        csb::static_array<custom, 2> cexpected;
        moveable(c, cexpected);

        cexpected[1] = c[1] = custom(42);
        moveable(c, cexpected);
    }

    using namespace csb;
    SCENARIO("Range init")
    {
        GIVEN("A Range of elements a, b, c")
        {
            auto gen = data_generator();
            auto a = gen();
            auto b = gen();
            auto c = gen();
            decltype(gen()) abc[] = {a, b, c};

            WHEN("Initializing a static_array of size 3")
            {
                auto sut = create_sut<3>(abc);
                THEN("Array has element a, b and c in order")
                {
                    REQUIRE(sut[0] == a);
                    REQUIRE(sut[1] == b);
                    REQUIRE(sut[2] == c);
                }
            }
            WHEN("Initializing a static_array of size 2")
            {
                auto sut = create_sut<2>(abc);
                THEN("Array has element a, b in order")
                {
                    REQUIRE(sut[0] == a);
                    REQUIRE(sut[1] == b);
                }
            }
            WHEN("Initializing a static_array of size 4")
            {
                auto sut = create_sut<4>(abc);
                THEN("Array has element a, b and c in order")
                {
                    REQUIRE(sut[0] == a);
                    REQUIRE(sut[1] == b);
                    REQUIRE(sut[2] == c);
                }
            }
        }
    }

    SCENARIO("operator[]")
    {
        constexpr csb::static_array<int, 3> arr{1, 2, 3};
        constexpr int a = arr[1];
        static_assert(a == 2, "grr");
    }

    SCENARIO("at")
    {
        constexpr csb::static_array<int, 3> arr{1, 2, 3};
        constexpr int a = arr.at(1);
        static_assert(a == 2, "grr");
    }

    SCENARIO("fill ctor")
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
} // namespace test