#pragma once

#include <functional>
#include <type_traits>

namespace csb
{
    namespace ccept
    {
        template <typename T, typename = void> struct value_type
        {
        };

        template <typename T>
        struct value_type<T, std::void_t<typename T::value_type>>
        {
            using type = typename T::value_type;
        };
        template <typename T>
        struct value_type<T, std::void_t<typename T::value_t>>
        {
            using type = typename T::value_t;
        };

        template <typename T> struct value_type<T *>
        {
            using type = T;
        };
        template <typename T> struct value_type<T const*>
        {
            using type = T;
        };

        template <typename T>
        using value_t = typename value_type<std::remove_reference_t<T>>::type;

        template <typename T, typename U> concept bool Same()
        {
            return std::is_same_v<T, U>;
        }

        template <typename T> concept bool DefaultConstructible()
        {
            return std::is_default_constructible_v<T>;
        }

        template <typename T> concept bool CopyConstructible()
        {
            return std::is_copy_constructible_v<T>;
        }

        template <typename T> concept bool CopyAssignable()
        {
            return std::is_copy_assignable_v<T>;
        }

        template <typename T> concept bool Copyable()
        {
            return CopyConstructible<T>() && CopyAssignable<T>();
        }

        template <typename T> concept bool Semiregular()
        {
            return DefaultConstructible<T>() && Copyable<T>();
        }

        template <typename R, typename L> concept bool EqualityComparable()
        {
            return requires(R r, L l)
            {
                // clang-format off
                {r == l}->bool;
                {r != l}->bool;
                {l == r}->bool;
                {l != r}->bool;
                // clang-format on
            };
        }

        template <typename T> concept bool EqualityComparable()
        {
            return EqualityComparable<T, T>();
        }

        template <typename T> concept bool Regular()
        {
            return Semiregular<T>() && EqualityComparable<T>();
        }

        //=============================================================================
        // range related
        //=============================================================================
        template <typename T> concept bool Incrementable()
        {
            return requires(T it)
            {
                // clang-format off
                {++it}->T &;
                {it++}->T;
                // clang-format on
            };
        }

        template <typename T> concept bool Iterator()
        {
            return EqualityComparable<T>() 
            && Incrementable<T>() 
            && requires(T it)
            {
                typename value_t<T>;
                // clang-format off
                {*it}->value_t<T> const &;
                // clang-format on
            };
        }

        template <typename T, typename Iter> concept bool Sentinel()
        {

            return EqualityComparable<T, Iter>();
        }

        template <typename T>
        using iterator_t = decltype(std::begin(std::declval<T>()));
        template <typename T>
        using sentinel_t = decltype(std::end(std::declval<T>()));

        namespace impl
        {
            using std::begin;
            using std::end;

            template <typename T> concept bool Range()
            {
                return requires(T & t)
                {
                    // clang-format off
                    {begin(t)};
                    {end(t)};
                    // clang-format oon
                    Iterator<decltype(begin(t))>();
                    Sentinel<decltype(begin(t)), decltype(end(t))>();
                };
            }
        }

        template <typename T> concept bool Range() { return impl::Range<T>(); }

        //=============================================================================
        // Higher order functions
        //=============================================================================

        template <typename T> struct function_traits;
        template <typename R, typename... Args>
        struct function_traits<R(Args...)>
        {
            using signature_t = R(Args...);
            using return_t = R;
            using args_t = std::tuple<Args...>;
        };

        template <typename T>
        using signature_t = typename function_traits<T>::signature_t;
        template <typename T>
        using signature_return_t = typename function_traits<T>::return_t;
        template <typename T>
        using signature_args_t = typename function_traits<T>::args_t;

        template <typename T, typename Sig> concept bool Invokable()
        {
            return requires(T t, signature_args_t<Sig> args)
            {
                typename signature_t<Sig>;
                typename signature_return_t<Sig>;
                // clang-format off
                {std::apply(t, std::move(args))}
                    ->signature_return_t<Sig>;
                // clang-format on
            };
        }

        //=============================================================================
        // utility
        //=============================================================================

        template <typename T> concept bool Streamable()
        {
            return requires(std::ostream & os, T t){{os << t}->std::ostream & };
        }
    }
}
