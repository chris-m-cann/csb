#include "singly_linked_list.hpp"

#include <catch2/catch.hpp>

#include <random>
#include <type_traits>
#include <vector>

namespace csb::test
{

    namespace
    {
        template <typename... Args> auto create_sut(Args &&... args)
        {
            using common_t = std::common_type_t<Args...>;
            auto sut = singly_linked_list<common_t>();
            sut.append(std::vector{std::forward<Args>(args)...});
            return sut;
        }
    } // namespace

    SCENARIO("singly_linked_list::push_front")
    {
        {
            GIVEN("a list of size s")
            {
                int const order[] = {1, 2, 3, 4};
                auto sut = singly_linked_list<int>();
                sut.append(order);

                auto const s = sut.size();

                WHEN("pushing a new element on the front")
                {
                    auto const v = 9;

                    auto const ret = sut.push_front(v);

                    THEN("size == s + 1") { REQUIRE(sut.size() == s + 1); }

                    THEN("ret is an iterator pointing to the new element")
                    {
                        REQUIRE(*ret == v);
                    }

                    THEN("begin points to the new element")
                    {
                        REQUIRE(sut.begin() == ret);
                    }

                    THEN("order of following elements is unchanged")
                    {
                        auto it = sut.begin();
                        for (auto const &o : order)
                        {
                            REQUIRE(*++it == o);
                        }
                    }
                }
            }
        }
    }

    SCENARIO("singly_linked_list::push_back")
    {
        GIVEN("a list of size s")
        {
            int const order[] = {1, 2, 3, 4};
            auto sut = singly_linked_list<int>();
            sut.append(order);

            auto const s = sut.size();

            WHEN("pushing a new element on the back")
            {
                auto const v = 9;

                auto ret = sut.push_back(v);

                THEN("size == s + 1") { REQUIRE(sut.size() == s + 1); }

                THEN("ret is an iterator pointing to the new element")
                {
                    REQUIRE(*ret == v);
                }

                THEN("last element before tail points to new element")
                {
                    auto forward = sut.begin();
                    auto trailing = forward;
                    while (++forward != sut.end())
                        trailing = forward;

                    REQUIRE(trailing == ret);
                    REQUIRE(++ret == sut.end());
                }

                THEN("order of following elements is unchanged")
                {
                    auto it = sut.begin();
                    for (auto const &o : order)
                    {
                        REQUIRE(*it++ == o);
                    }
                }
            }
        }
        GIVEN("an empty list")
        {
            auto sut = singly_linked_list<int>();

            WHEN("calling push_back")
            {
                auto const v = 42;
                sut.push_back(v);

                THEN("list contains 1 element") { REQUIRE(sut.size() == 1); }
                THEN("begin points to the new element")
                {
                    REQUIRE(*sut.begin() == v);
                }
            }
        }
    }

    SCENARIO("singly_linked_list::iteration")
    {
        GIVEN("empty list")
        {
            auto sut = singly_linked_list<int>();

            THEN("begin == end") { REQUIRE(sut.begin() == sut.end()); }

            WHEN("x elements are added")
            {
                int const vs[] = {5, 2, 7};

                for (auto v : vs)
                {
                    sut.push_back(v);
                }

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

    SCENARIO("singly_linked_list::insert")
    {
        GIVEN("list with n elements in")
        {
            auto sut = create_sut(1, 2, 3);

            auto const n = 3;

            WHEN("giving insert begin")
            {
                auto new_element = 4;
                auto ret_pos = sut.insert(sut.begin(), new_element);
                THEN("new element is at front of list")
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
                auto new_element = -5;
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
                auto new_element = 42;
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

    SCENARIO("singly_linked_list::append")
    {
        GIVEN("an empty list")
        {
            auto sut = singly_linked_list<std::string>();

            WHEN("I append a range of n values")
            {
                std::string const input[] = {"one", "two", "three"};

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
                std::vector<std::string> empty;
                sut.append(empty);
                THEN("list still empty")
                {
                    REQUIRE(sut.is_empty());
                    REQUIRE(sut.size() == 0);
                    REQUIRE(sut.empty());
                }
            }
        }
        GIVEN("a non empty list")
        {
            auto sut = singly_linked_list<double>();
            double prior[] = {3.14, 2.67, -6.66};

            for (auto e : prior)
            {
                sut.push_back(e);
            }

            WHEN("appending n more elements")
            {
                double appended[] = {1.11, 10.0, 5};
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

    SCENARIO("singly_linked_list::insert_after")
    {
        GIVEN("a list containing 2 elements")
        {
            auto sut = singly_linked_list<char>();
            auto first = 'a';
            auto second = 'Z';
            auto list = {first, second};
            sut.append(list);

            WHEN("inserting after begin")
            {
                auto target = 't';
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
        }
    }

    SCENARIO("singly_linked_list::pop_front")
    {
        GIVEN("a list with n elements in")
        {
            auto sut = singly_linked_list<int>();
            int const data[] = {1, 4, 6, 9};

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
        GIVEN("an empty list")
        {
            auto sut = singly_linked_list<int>();

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

    SCENARIO("singly_linked_list::pop_back")
    {
        GIVEN("a list with n elements in")
        {
            auto sut = singly_linked_list<int>();
            int const data[] = {1, 4, 6, 9};

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
        GIVEN("an empty list")
        {
            auto sut = singly_linked_list<int>();
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

    SCENARIO("singly_linked_list::erase")
    {
        GIVEN("a list with n elements in")
        {
            auto sut = singly_linked_list<int>();
            int const data[] = {1, 4, 6, 9};

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
                THEN("the element after the removed one is now in the "
                     "previous "
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
        GIVEN("an empty list")
        {
            auto sut = singly_linked_list<int>();

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

    SCENARIO("singly_linked_list::reverse")
    {
        GIVEN("a list of several values")
        {
            auto sut = singly_linked_list<int>();
            int const in[] = {1, 4, 6, 9};

            sut.append(in);

            WHEN("reversing the list")
            {
                sut.reverse();

                THEN("the list contains the same elements but in reverse "
                     "order")
                {
                    std::vector<int> v(sut.begin(), sut.end());
                    std::vector reversed{9, 6, 4, 1};

                    REQUIRE_THAT(
                        v, Catch::Matchers::Vector::EqualsMatcher(reversed));
                }
            }
        }
        GIVEN("a list of containing 1 value")
        {
            auto sut = singly_linked_list<int>();
            auto in = 1;
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
        GIVEN("empty range")
        {
            auto sut = singly_linked_list<int>();
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

    SCENARIO("singly_linked_list::equality")
    {

        // reflexive
        GIVEN("a list a")
        {
            auto a = singly_linked_list<int>();

            THEN("a == a") { REQUIRE(a == a); }

            a.append(std::vector{1, 2, 3});

            THEN("a == a") { REQUIRE(a == a); }
        }

        // symmetric
        GIVEN("2 lists a and b")
        {
            auto a = singly_linked_list<int>();
            auto b = singly_linked_list<int>();

            WHEN("a == b")
            {
                REQUIRE(a == b);
                THEN("b == a") { REQUIRE(b == a); }

                a.append(std::vector{1, 2, 3});
                b.append(std::vector{1, 2, 3});

                REQUIRE(a == b);
                THEN("b == a") { REQUIRE(b == a); }
            }
        }

        // transitive
        GIVEN("3 lists a, b and c")
        {
            auto a = singly_linked_list<int>();
            auto b = singly_linked_list<int>();
            auto c = singly_linked_list<int>();

            WHEN("a == b and b == c")
            {
                REQUIRE(a == b);
                REQUIRE(b == c);
                THEN("a == c") { REQUIRE(a == c); }

                a.append(std::vector{1, 2, 3});
                b.append(std::vector{1, 2, 3});
                c.append(std::vector{1, 2, 3});

                REQUIRE(a == b);
                REQUIRE(b == c);
                THEN("a == c") { REQUIRE(a == c); }
            }
        }

        // differnt sizes are !=
        GIVEN("two lists of different sizes")
        {
            auto a = singly_linked_list<int>();
            auto b = singly_linked_list<int>();

            auto v = 1;
            a.push_back(v);
            a.push_back(v);
            b.push_back(v);

            THEN("they are not equal")
            {
                REQUIRE(a != b);
                REQUIRE_FALSE(a == b);
            }
        }

        // different values are !=
        // different orders are !=
        GIVEN("two lists with the same size")
        {
            auto a = singly_linked_list<int>();
            auto b = singly_linked_list<int>();

            auto v1 = 1;
            auto v2 = 2;

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

    SCENARIO("singly_linked_list::merge")
    {
        GIVEN("two non-empty sorted lists")
        {
            auto a = singly_linked_list<int>();
            auto b = singly_linked_list<int>();

            int in[] = {1, 2, 4, 6, 8, 10};

            std::random_device rd;
            std::mt19937 randg(rd());
            std::uniform_int_distribution<> distr(0, 6);

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

                    auto merged = singly_linked_list<int>();
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

                    auto merged = singly_linked_list<int>();
                    merged.append(in);
                    REQUIRE(b == merged);
                }
            }
        }
        GIVEN("two lists. one empty, one not")
        {
            auto non_empty = singly_linked_list<int>();
            int const in[] = {3, 2, 6, 7};
            non_empty.append(in);

            auto empty = singly_linked_list<int>();
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
        GIVEN("two empty lists a and b")
        {
            auto a = singly_linked_list<int>();
            auto b = singly_linked_list<int>();

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

    SCENARIO("singly_linked_list::apply_in_reverse")
    {
        GIVEN("a list of values")
        {
            auto sut = singly_linked_list<std::string>();
            std::string in[] = {"one", "two", "three"};

            sut.append(in);

            WHEN("calling apply_inverse from begin->end")
            {
                std::vector<std::string> sequence;
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
        GIVEN("empty range")
        {
            auto sut = singly_linked_list<std::string>();
            bool called = false;
            apply_in_reverse(
                sut.begin(), sut.end(), [&](auto) { called = true; });

            THEN("functor not called") { REQUIRE_FALSE(called); }
        }
    }

    SCENARIO("singly_linked_list::copyable")
    {
        GIVEN("a list a")
        {
            auto a = create_sut(1, 2, 3, 4, 5);
            WHEN("copy constructing b from a")
            {
                decltype(a) b(a);

                THEN("a == b") { REQUIRE(a == b); }

                WHEN("altering b")
                {
                    ++(*b.begin());
                    THEN("a != b") { REQUIRE(a != b); }
                }

                WHEN("altering a")
                {
                    ++(*a.begin());
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
                    ++(*b.begin());
                    THEN("a != b") { REQUIRE(a != b); }
                }

                WHEN("altering a")
                {
                    ++(*a.begin());
                    THEN("a != b") { REQUIRE(a != b); }
                }
            }
        }
    }

    SCENARIO("singly_linked_list::moveable")
    {
        GIVEN("2 equal lists a and b")
        {
            auto a = create_sut(1, 3, 5, 7, 9);
            auto b = create_sut(1, 3, 5, 7, 9);

            WHEN("move constructing a third list c from b")
            {
                decltype(a) c(std::move(b));
                THEN("c == a") { REQUIRE(c == a); }
            }

            WHEN("move assigning to a third list c from b")
            {
                decltype(a) c = create_sut(5, 6, 7, 8, 9);

                c = std::move(b);

                THEN("c == a") { REQUIRE(c == a); }
            }
        }
    }
} // namespace csb::test