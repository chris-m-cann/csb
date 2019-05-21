#include "singly_linked_list.hpp"

#include <core/is_regular_test.hpp>
#include <core/test_util.hpp>

#include <catch/catch.hpp>

#include <iostream>
#include <random>
#include <type_traits>
#include <vector>

namespace test
{

    namespace
    {
        struct int_generator
        {
            using sut_t = csb::singly_linked_list<int>;
            using data_t = int;

            sut_t create_sut() const { return {}; }
            int_data_generator data_source() const { return {}; }
        };

        struct custom_generator
        {
            using sut_t = csb::singly_linked_list<custom>;
            using data_t = custom;

            sut_t create_sut() const { return {}; }
            custom_data_generator data_source() const { return {}; }
        };
    } // namespace

    template <> struct arg_printer<csb::singly_linked_list<int>>
    {
        static std::ostream &print(std::ostream &os,
                                   csb::singly_linked_list<int> const &li)
        {
            os << "[";
            for (auto const &i : li)
            {
                os << i << ",";
            }
            os << (li.is_empty() ? "]" : "\b]");
            return os;
        }
    };

    template <> struct arg_printer<csb::singly_linked_list<custom>>
    {
        static std::ostream &print(std::ostream &os,
                                   csb::singly_linked_list<custom> const &lc)
        {
            os << "singly_linked_list<custom>[";
            for (auto const &c : lc)
            {
                os << c.i << ",";
            }
            os << (lc.is_empty() ? "]" : "\b]");
            return os;
        }
    };

    template <SutGenerator G> auto push_front(G const &generator)
    {
        GIVEN_T("list of size s", G)
        {
            auto sut = generator.create_sut();
            auto gen = generator.data_source();
            data_t<G> order[] = {gen(), gen(), gen()};

            sut.append(order);

            auto s = sut.size();

            WHEN("pushing new element on front")
            {
                auto first = gen();
                auto return_v = sut.push_front(first);

                THEN("size == s + 1") { REQUIRE(sut.size() == s + 1); }

                THEN("returned iterator points to new element")
                {
                    REQUIRE(*return_v == first);
                }

                THEN("begin points to new element")
                {
                    REQUIRE(sut.begin() == return_v);
                }

                THEN("relative order of other elements is unchanged")
                {
                    auto it = sut.begin();
                    REQUIRE(*++it == order[0]);
                    REQUIRE(*++it == order[1]);
                    REQUIRE(*++it == order[2]);
                }
            }
        }
    }

    template <SutGenerator G> auto push_back(G g)
    {
        GIVEN_T("list of size s", G)
        {
            auto sut = g.create_sut();
            auto generator = g.data_source();
            auto v1 = generator();
            auto v2 = generator();
            auto v3 = generator();

            sut.push_back(v1);
            sut.push_back(v2);
            sut.push_back(v3);

            auto s = sut.size();

            WHEN("pushing new element on back")
            {
                auto test_v = generator();
                auto return_v = sut.push_back(test_v);

                THEN("size == s + 1") { REQUIRE(sut.size() == s + 1); }

                THEN("returned iterator points to new element")
                {
                    REQUIRE(return_v != sut.end());
                    REQUIRE(*return_v == test_v);
                }

                THEN("last element before tail points to new element")
                {
                    auto forward = sut.begin();
                    auto trailing = forward;
                    while (++forward != sut.end())
                        trailing = forward;

                    REQUIRE(trailing == return_v);
                    REQUIRE(++return_v == sut.end());
                }
                THEN("relative order of other elements is unchanged")
                {
                    auto it = sut.begin();
                    REQUIRE(*it++ == v1);
                    REQUIRE(*it++ == v2);
                    REQUIRE(*it++ == v3);
                    REQUIRE(*it++ == test_v);
                }
            }
        }
        GIVEN_T("an empty list", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            WHEN("calling push_back")
            {
                auto v = gen();
                sut.push_back(v);

                THEN("list contains 1 element") { REQUIRE(sut.size() == 1); }
                THEN("begin points to the new element")
                {
                    REQUIRE(*sut.begin() == v);
                }
            }
        }
    }

    template <SutGenerator G> auto iteration(G g)
    {
        GIVEN_T("empty list", G)
        {
            auto sut = g.create_sut();

            THEN("begin == end") { REQUIRE(sut.begin() == sut.end()); }

            WHEN("x elements are added")
            {
                auto generator = g.data_source();
                data_t<G> vs[] = {generator(), generator(), generator()};

                for (auto v : vs)
                {
                    sut.push_back(v);
                }
                auto x = sut.size();

                THEN("range begin -> end contains all added elements")
                {
                    int counts[3] = {0, 0, 0};
                    for (auto elem : sut)
                    {
                        for (int i = 0; i != 3; ++i)
                        {
                            if (elem == vs[i])
                            {
                                counts[i]++;
                            }
                        }
                    }
                    REQUIRE(counts[0] == 1);
                    REQUIRE(counts[1] == 1);
                    REQUIRE(counts[2] == 1);
                }
            }
        }
    }

    template <SutGenerator G> auto insert(G g)
    {
        GIVEN_T("list with n elements in", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            data_t<G> input[] = {gen(), gen(), gen()};

            const auto n = 3;

            sut.append(input);

            WHEN("giving insert begin")
            {
                auto new_element = gen();
                auto ret_pos = sut.insert(sut.begin(), new_element);
                THEN("new element is at fron of list")
                {
                    REQUIRE(*sut.begin() == new_element);
                }
                THEN("return points to new element")
                {
                    REQUIRE(ret_pos == sut.begin());
                }
                THEN("size is n + 1") { REQUIRE(sut.size() == n + 1); }
            }
            WHEN("giving insert end")
            {
                auto new_element = gen();
                auto ret_pos = sut.insert(sut.end(), new_element);
                THEN("new element is last in list")
                {
                    REQUIRE(new_element == *last(sut));
                    REQUIRE(++ret_pos == std::end(sut));
                }

                THEN("return points to new element")
                {
                    REQUIRE(ret_pos == last(sut));
                }
                THEN("size is n + 1") { REQUIRE(sut.size() == n + 1); }
            }
            WHEN("inserting at neither begin or end")
            {
                auto new_element = gen();
                auto pos_in = ++sut.begin();
                auto ret_pos = sut.insert(pos_in, new_element);
                THEN("new element is before the position passed in")
                {
                    REQUIRE(++ret_pos == pos_in);
                }
                THEN("return points to new element")
                {
                    REQUIRE(new_element == *ret_pos);
                }
                THEN("size is n + 1") { REQUIRE(sut.size() == n + 1); }
            }
        }
    }

    template <SutGenerator G> auto append(G g)
    {
        GIVEN_T("i have an empty list", G)
        {
            auto sut = g.create_sut();
            WHEN("I append a range of n values")
            {
                auto gen = g.data_source();
                data_t<G> input[] = {gen(), gen(), gen()};

                const auto n = 3;
                sut.append(input);

                THEN("all of those elements are added to the list")
                {
                    REQUIRE(sut.size() == 3);
                    int i = 0;
                    auto it = sut.begin();
                    do
                    {
                        REQUIRE(*it++ == input[i]);
                    } while (++i != 3);
                }
                THEN("list will be of size n") { REQUIRE(sut.size() == n); }
            }
            WHEN("i append an empy range")
            {
                std::vector<data_t<G>> empty;
                sut.append(empty);
                THEN("list still empty")
                {
                    REQUIRE(sut.is_empty());
                    REQUIRE(sut.size() == 0);
                    REQUIRE(sut.empty());
                }
            }
        }
        GIVEN_T("a non empty list", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            data_t<G> prior[] = {gen(), gen(), gen()};

            for (auto e : prior)
            {
                sut.push_back(e);
            }

            WHEN("appending n more elements")
            {
                data_t<G> appended[] = {gen(), gen(), gen()};
                const auto n = 3;
                const auto prev_size = sut.size();

                sut.append(appended);
                THEN("new data appears after old")
                {
                    REQUIRE(sut.size() == 6);
                    auto beg = sut.begin();
                    REQUIRE(*beg++ == prior[0]);
                    REQUIRE(*beg++ == prior[1]);
                    REQUIRE(*beg++ == prior[2]);
                    REQUIRE(*beg++ == appended[0]);
                    REQUIRE(*beg++ == appended[1]);
                    REQUIRE(*beg++ == appended[2]);
                    REQUIRE(beg == sut.end());
                }
                THEN("size is n larger than before")
                {
                    REQUIRE(sut.size() == prev_size + n);
                }
            }
        }
    }

    template <SutGenerator G> auto insert_after(G g)
    {
        GIVEN_T("a list containing 2 elements", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            auto first = gen();
            auto second = gen();
            auto list = {first, second};
            sut.append(list);

            WHEN("inserting after begin")
            {
                auto target = gen();
                auto ret = sut.insert_after(sut.begin(), target);
                THEN("the new element is at position ++beign")
                {
                    REQUIRE(*++sut.begin() == target);
                }
                THEN("ret points to the newly added element")
                {
                    REQUIRE(ret == ++sut.begin());
                }

                THEN("previous element is not effected")
                {
                    REQUIRE(*sut.begin() == first);
                }
                THEN("following element is not altered")
                {
                    REQUIRE(*++ret == second);
                }
                THEN("size will be 3") { REQUIRE(sut.size() == 3); }
            }
            WHEN("inserting at end")
            {
                THEN("crash")
                {
                    // how to test??
                }
            }
        }
        GIVEN_T("an empty list", G)
        {
            WHEN("inserting after begin")
            {
                THEN("crash")
                {
                    // test??
                }
            }
        }
    }

    template <SutGenerator G> auto pop_front(G g)
    {
        GIVEN_T("a list with n elements in", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            data_t<G> data[] = {gen(), gen(), gen()};

            sut.append(data);
            const auto n = sut.size();

            WHEN("popping from front")
            {
                auto second = ++sut.begin();
                sut.pop_front();
                THEN("size = n-1") { REQUIRE(sut.size() == n - 1); }
                THEN("begin points to previous ++begin")
                {
                    REQUIRE(sut.begin() == second);
                }
            }
        }
        GIVEN_T("an empty list", G)
        {
            auto sut = g.create_sut();
            WHEN("popping front")
            {
                sut.pop_front();
                THEN("is empty")
                {
                    REQUIRE(sut.empty());
                    REQUIRE(sut.is_empty());
                    REQUIRE(sut.size() == 0);
                }
            }
        }
    }

    template <SutGenerator G> auto pop_back(G g)
    {
        GIVEN_T("a list with n elements in", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            data_t<G> data[] = {gen(), gen(), gen(), gen()};

            sut.append(data);
            const auto n = sut.size();

            WHEN("popping from back")
            {
                auto third = ++(++sut.begin());
                sut.pop_back();
                THEN("size = n-1") { REQUIRE(sut.size() == n - 1); }
                THEN("the last element is now the third")
                {
                    REQUIRE(last(sut) == third);
                }
            }
        }
        GIVEN_T("an empty list", G)
        {
            auto sut = g.create_sut();
            WHEN("popping back")
            {
                sut.pop_back();
                THEN("is empty")
                {
                    REQUIRE(sut.empty());
                    REQUIRE(sut.is_empty());
                    REQUIRE(sut.size() == 0);
                }
            }
        }
    }

    template <SutGenerator G> auto erase(G g)
    {
        GIVEN_T("a list with n elements in", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            data_t<G> data[] = {gen(), gen(), gen(), gen()};

            sut.append(data);
            const auto n = sut.size();

            WHEN("erasing last elemnent")
            {
                auto third = ++(++sut.begin());

                auto one_after = sut.erase(last(sut));
                THEN("size = n-1") { REQUIRE(sut.size() == n - 1); }
                THEN("the last element is now the third")
                {
                    REQUIRE(last(sut) == third);
                }
                THEN("returned iterator points to end")
                {
                    REQUIRE(one_after == sut.end());
                }
            }

            WHEN("erasing first elemnent")
            {
                auto second = ++sut.begin();
                auto one_after = sut.erase(sut.begin());

                THEN("size = n-1") { REQUIRE(sut.size() == n - 1); }
                THEN("the second element is now the first")
                {
                    REQUIRE(std::begin(sut) == second);
                }
                THEN("returned iterator points to new begin")
                {
                    REQUIRE(sut.begin() == one_after);
                }
            }

            WHEN("erasing a middle elemnent")
            {
                auto forth = ++(++sut.begin());
                auto third = forth++;
                auto one_after = sut.erase(third);
                THEN("size = n-1") { REQUIRE(sut.size() == n - 1); }
                THEN("the element after the removed one is now in the previous "
                     "position")
                {
                    auto new_third = ++(++sut.begin());
                    REQUIRE(new_third == forth);
                }
                THEN("returned iterator points element now in parameters "
                     "position")
                {
                    REQUIRE(one_after == forth);
                }
            }
        }
        GIVEN_T("an empty list", G)
        {
            auto sut = g.create_sut();
            WHEN("erasing begin")
            {
                sut.erase(sut.begin());
                THEN("is empty")
                {
                    REQUIRE(sut.empty());
                    REQUIRE(sut.is_empty());
                    REQUIRE(sut.size() == 0);
                }
            }
        }
    }

    template <SutGenerator G> auto reverse(G g)
    {
        GIVEN_T("a list of several values", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            data_t<G> in[] = {gen(), gen(), gen()};
            sut.append(in);

            WHEN("reversing the list")
            {
                sut.reverse();

                THEN("the list contains the same elements but in reverse order")
                {
                    auto revd = sut.begin();
                    auto rin = std::rbegin(in);
                    REQUIRE(sut.size() == 3);
                    for (; revd != sut.end(); ++revd, ++rin)
                    {
                        REQUIRE(*revd == *rin);
                    }
                }
            }
        }
        GIVEN_T("a list of containing 1 value", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            auto in = gen();
            sut.push_back(in);

            WHEN("calling reverse on the list")
            {
                sut.reverse();

                THEN("the list is unchanged")
                {
                    auto revd = sut.begin();
                    REQUIRE(sut.size() == 1);
                    REQUIRE(*revd == in);
                }
            }
        }
        GIVEN_T("empty range", G)
        {
            auto sut = g.create_sut();
            WHEN("attempting to reverse the list")
            {
                sut.reverse();
                THEN("the list is still empty")
                {
                    REQUIRE(sut.empty());
                    REQUIRE(sut.is_empty());
                }
            }
        }
    }

    template <SutGenerator G> auto equality(G g)
    {
        // empty list is reflexive and equla to itself and transitive
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

        not_empty.append(in);
        also_not_empty.append(in);
        not_empty_as_well.append(in);

        // differnt sizes are !=
        GIVEN_T("two lists of different sizes", G)
        {
            auto a = g.create_sut();
            auto b = g.create_sut();

            auto v = gen();
            a.push_back(v);
            a.push_back(v);
            b.push_back(v);
            THEN("they are not equal") { REQUIRE(a != b); }
        }

        // different values are !=
        // different orders are !=
        GIVEN_T("two lists with the same size", G)
        {
            auto a = g.create_sut();
            auto b = g.create_sut();

            auto gen = g.data_source();
            auto v1 = gen();
            auto v2 = gen();

            WHEN("the values contained are different")
            {
                a.push_back(v1);
                b.push_back(v2);
                THEN("they are not equal") { REQUIRE(a != b); }
            }
            WHEN("with equal nodes in different orders")
            {
                auto order1 = {v1, v2};
                auto order2 = {v2, v1};
                a.append(order1);
                b.append(order2);
                THEN("they are not equal") { REQUIRE(a != b); }
            }
        }
    }

    template <SutGenerator G> auto merge(G g)
    {
        GIVEN_T("two non-empty sorted list", G)
        {
            auto a = g.create_sut();
            auto b = g.create_sut();
            auto gen = g.data_source();
            data_t<G> in[] = {
                gen(), gen(), gen(), gen(), gen(), gen(), gen(), gen()};

            std::random_device rd;
            std::mt19937 randg(rd());
            std::uniform_int_distribution<> distr(0, 8);

            std::shuffle(std::begin(in), std::end(in), randg);
            auto pivot = distr(randg);

            std::sort(std::begin(in), std::begin(in) + pivot);
            std::sort(std::begin(in) + pivot, std::end(in));

            for (auto it = std::begin(in); it != std::begin(in) + pivot; ++it)
            {
                a.push_back(*it);
            }
            for (auto it = std::begin(in) + pivot; it != std::end(in); ++it)
            {
                b.push_back(*it);
            }

            WHEN("merging the b into a")
            {
                a.merge(std::move(b));

                THEN("the resultant list is sorted")
                {
                    std::sort(std::begin(in), std::end(in));

                    auto merged = g.create_sut();
                    merged.append(in);
                    REQUIRE(a == merged);
                }
            }
            WHEN("merging the a into b")
            {
                b.merge(std::move(a));

                THEN("the resultant list is sorted")
                {
                    std::sort(std::begin(in), std::end(in));

                    auto merged = g.create_sut();
                    merged.append(in);
                    REQUIRE(b == merged);
                }
            }
        }
        GIVEN_T("two lists. one empty, one not", G)
        {
            auto non_empty = g.create_sut();
            auto gen = g.data_source();
            data_t<G> in[] = {gen(), gen(), gen()};
            non_empty.append(in);

            auto empty = g.create_sut();

            WHEN("merging the empty into the non-empty")
            {
                auto cpy = non_empty;
                non_empty.merge(std::move(empty));
                THEN("the list remains unchanged")
                {
                    REQUIRE(cpy == non_empty);
                }
            }
            WHEN("merging the non-empty into the empy")
            {
                auto cpy = non_empty;
                empty.merge(std::move(cpy));

                THEN("both lists are now equal")
                {
                    REQUIRE(empty == non_empty);
                }
            }
        }
        GIVEN_T("two empty lists a and b", G)
        {
            auto a = g.create_sut();
            auto b = g.create_sut();

            WHEN("merging a into b")
            {
                b.merge(std::move(a));
                THEN("b remains empty") { REQUIRE(b.is_empty()); }
            }
            WHEN("merging b into a")
            {
                a.merge(std::move(b));
                THEN("a remains empty") { REQUIRE(a.is_empty()); }
            }
        }
    }

    template <SutGenerator G> auto apply_in_reverse(G g)
    {
        GIVEN_T("a list of values", G)
        {
            auto sut = g.create_sut();
            auto gen = g.data_source();
            data_t<G> in[] = {gen(), gen(), gen()};
            sut.append(in);

            WHEN("calling apply_inverse from begin->end")
            {
                std::vector<data_t<G>> sequence;
                apply_in_reverse(sut.begin(), sut.end(), [&](auto elem) {
                    sequence.push_back(elem);
                });
                THEN("the element are passed in reverse sequence to in the "
                     "list to the functor")
                {
                    REQUIRE(std::equal(std::begin(in),
                                       std::end(in),
                                       std::rbegin(sequence),
                                       std::rend(sequence)));
                }
            }
        }
        GIVEN_T("empty range", G)
        {
            auto sut = g.create_sut();
            bool called = false;
            apply_in_reverse(
                sut.begin(), sut.end(), [&](auto) { called = true; });

            THEN("functor not called") { REQUIRE_FALSE(called); }
        }
    }

    SCENARIO("apply_in_reverse")
    {
        apply_in_reverse(int_generator());
        apply_in_reverse(custom_generator());
    }

    SCENARIO("singly linked list push_front")
    {
        push_front(int_generator());
        push_front(custom_generator());
    }

    SCENARIO("push back")
    {
        push_back(int_generator());
        push_back(custom_generator());
    }

    SCENARIO("iteration")
    {
        iteration(int_generator());
        iteration(custom_generator());
    }

    SCENARIO("insert")
    {
        insert(int_generator());
        insert(custom_generator());
    }

    SCENARIO("append")
    {
        append(int_generator());
        append(custom_generator());
    }

    SCENARIO("insert after")
    {
        insert_after(int_generator());
        insert_after(custom_generator());
    }

    SCENARIO("pop_front")
    {
        pop_front(int_generator());
        pop_front(custom_generator());
    }

    SCENARIO("pop_back")
    {
        pop_back(int_generator());
        pop_back(custom_generator());
    }

    SCENARIO("erase")
    {
        erase(int_generator());
        erase(custom_generator());
    }

    SCENARIO("reverse list")
    {
        reverse(int_generator());
        reverse(custom_generator());
    }

    SCENARIO("linked list equality")
    {
        equality(int_generator());
        equality(custom_generator());
    }

    SCENARIO("linked list copyable")
    {
        csb::singly_linked_list<int> i;
        copyable(i,
                 [](auto &list) { list.push_back(2); },
                 "append to empty (singly_linked_list<int>)");
        i.push_back(12);
        copyable(i,
                 [](auto &list) { *list.begin() = 42; },
                 "modify single element (singly_linked_list<int>)");

        csb::singly_linked_list<custom> c;
        copyable(c,
                 [](auto &list) { list.push_back(custom(2)); },
                 "append to empty (singly_linked_list<custom>)");
        c.push_back(custom(12));
        copyable(c,
                 [](auto &list) { *list.begin() = custom(42); },
                 "modify single element (singly_linked_list<custom>)");
    }

    SCENARIO("linked list moveable")
    {
        csb::singly_linked_list<int> i;
        csb::singly_linked_list<int> expected;
        moveable(i, expected);

        i.push_back(42);
        expected.push_back(42);
        moveable(i, expected);

        csb::singly_linked_list<custom> c;
        csb::singly_linked_list<custom> cexpected;
        moveable(c, cexpected);

        c.push_back(custom(42));
        cexpected.push_back(custom(42));
        moveable(c, cexpected);
    }

    SCENARIO("merging 2 sorted linked lists")
    {
        merge(int_generator());
        merge(custom_generator());
    }
}