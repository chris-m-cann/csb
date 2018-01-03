#pragma once

#include "concepts.hpp"

#include <catch/catch.hpp>

#include <sstream>

#include <cxxabi.h>

namespace test
{
    // to be specialized by classes looking to override the function
    template <typename T> struct arg_printer;

    namespace impl
    {

        template <typename T> concept bool PrintArgable()
        {
            return requires(std::ostream & os, T t){
                {arg_printer<T>::print(os, t)}->std::ostream & };
        }

        template <typename T> struct print_arg_placeholder
        {
            print_arg_placeholder(T const &t) : arg(t) {}

            T const &arg;

            friend std::ostream &operator<<(std::ostream &os,
                                            print_arg_placeholder const &t)
            {
                return arg_printer<T>::print(os, t.arg);
            }
        };

        template <typename T> class type_name
        {
          public:
            type_name()
                  : _name(abi::__cxa_demangle(typeid(T).name(), nullptr,
                                              nullptr, nullptr))
            {
            }

            ~type_name() { free(_name); }

            char const *name() const { return _name; }

            friend std::ostream &operator<<(std::ostream &os,
                                            type_name const &t)
            {
                os << t._name;
                return os;
            }

          private:
            char *_name;
        };

        template <typename T> struct type_and_address_placeholder
        {
            type_and_address_placeholder(T const &t) : arg(t) {}

            T const &arg;

            friend std::ostream &
            operator<<(std::ostream &os, type_and_address_placeholder const &t)
            {
                os << (void *)&t << " (" << type_name<T>{} << ")";
                return os;
            }
        };

        template <typename T> decltype(auto) try_stream(T const &t)
        {
            if constexpr (csb::ccept::Streamable<T>())
            {
                return t;
            }
            else if constexpr (PrintArgable<T>())
            {
                return print_arg_placeholder(t);
            }
            else
            {
                return type_and_address_placeholder(t);
            }
        }

        template <typename... Args> std::string stringify(Args &&... args)
        {
            std::ostringstream oss;
            (oss << ... << try_stream(args));
            return oss.str();
        }
    }

    template <typename T> using sut_t = typename T::sut_t;
    template <typename T> using data_t = typename T::data_t;

    template <typename G> concept bool SutGenerator()
    {
        return requires(G g)
        {
            typename sut_t<G>;
            typename data_t<G>;
            // clang-format off
            {g.create_sut()}->sut_t<G>;
            {g.data_source()}->csb::ccept::Invokable<data_t<G>()>;
            //clang-format on
        };
    }

    template <typename T> impl::type_name<T> type_name() { return {}; }

    template <SutGenerator G> impl::type_name<sut_t<G>> type_name(G const &)
    {
        return {};
    }
}

#define GIVEN_V(msg, ...) GIVEN(test::impl::stringify(msg, __VA_ARGS__))
#define GIVEN_T(msg, type) GIVEN_V(msg, " (type=", type_name<G>(), ")")