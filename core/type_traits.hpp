#ifndef CSB_TYPE_TRAITS_HPP
#define CSB_TYPE_TRAITS_HPP

#include <cstddef>
#include <experimental/type_traits>
#include <type_traits>
#include <utility>

namespace csb
{
    template <typename T, typename = void> struct value_type
    {
    };

    template <typename T>
    struct value_type<T, std::void_t<typename T::value_type>>
    {
        using type = typename T::value_type;
    };

    template <typename T> struct value_type<T *>
    {
        using type = T;
    };
    template <typename T> struct value_type<T const *>
    {
        using type = T;
    };

    template <typename T>
    using value_t = typename value_type<std::remove_reference_t<T>>::type;

    template <typename T>
    constexpr bool is_copyable_v =
        std::is_copy_constructible_v<T> &&std::is_copy_assignable_v<T>;

    template <typename T>
    constexpr bool is_semi_regular_v =
        is_copyable_v<T> &&std::is_default_constructible_v<T>;

    namespace impl
    {
        template <bool b, typename T = void>
        using disable_if_t = std::enable_if_t<!b, T>;

        template <typename T, typename V>
        using equality_t = decltype(std::declval<T>() == std::declval<V>());

        template <typename T, typename V>
        using inequality_t = decltype(std::declval<T>() != std::declval<V>());

        template <typename T>
        using pre_increment_t =
            decltype(++(std::declval<std::remove_reference_t<T> &>()));

        template <typename T>
        using post_increment_t =
            decltype((std::declval<std::remove_reference_t<T> &>())++);

        template <typename T>
        using dereferrence_t = decltype(*(std::declval<T>()));

        using std::begin;
        using std::end;

        template <typename T>
        using make_begin_t = decltype(begin(std::declval<T>()));

        template <typename T>
        using begin_t =
            std::conditional_t<std::is_array_v<std::remove_reference_t<T>>,
                               std::decay_t<T>,
                               std::experimental::detected_t<make_begin_t, T>>;

        template <typename T>
        using make_end_t = decltype(begin(std::declval<T>()));

        template <typename T>
        using end_t =
            std::conditional_t<std::is_array_v<std::remove_reference_t<T>>,
                               std::decay_t<T>,
                               std::experimental::detected_t<make_end_t, T>>;

    } // namespace impl
    template <typename T, typename V = T>
    constexpr bool is_equality_comparable_v =
        std::experimental::is_detected_convertible_v<bool, impl::equality_t, T,
                                                     V> &&
            std::experimental::is_detected_convertible_v<bool, impl::equality_t,
                                                         V, T>
                &&std::experimental::is_detected_convertible_v<
                    bool, impl::inequality_t, T, V> &&std::experimental::
                    is_detected_convertible_v<bool, impl::inequality_t, V, T>;

    template <typename T>
    constexpr bool is_regular_v =
        is_semi_regular_v<T> &&is_equality_comparable_v<T>;

    // range based
    template <typename T>
    using iterator_t = decltype(std::begin(std::declval<T>()));
    template <typename T>
    using sentinel_t = decltype(std::end(std::declval<T>()));

    template <typename T>
    constexpr bool is_weakly_incrementable_v = is_semi_regular_v<T> &&
        // difference_type
        // difference type is a signed integral
        std::experimental::is_detected_convertible_v<
            std::add_lvalue_reference_t<std::remove_reference_t<T>> &,
            impl::pre_increment_t, T>
            &&std::experimental::is_detected_v<impl::post_increment_t, T>;

    template <typename T>
    constexpr bool is_dereferenceable_v = std::conjunction_v<
        std::experimental::is_detected<impl::dereferrence_t, T>,
        std::negation<std::experimental::is_detected_exact<
            void, impl::dereferrence_t, T>>>;

    template <typename T>
    constexpr bool is_iterator_v =
        is_dereferenceable_v<T> &&is_weakly_incrementable_v<T>;

    template <typename T, typename Iter>
    constexpr bool is_sentinel_v = is_equality_comparable_v<T, Iter>;

    template <typename T>
    constexpr bool is_range_v = std::conjunction_v<
        std::experimental::is_detected<impl::begin_t, T>,
        std::experimental::is_detected<impl::end_t, T>,
        std::bool_constant<
            is_iterator_v<std::experimental::detected_t<impl::begin_t, T>>>,
        std::bool_constant<
            is_sentinel_v<std::experimental::detected_t<impl::end_t, T>,
                          std::experimental::detected_t<impl::begin_t, T>>>>;

    //
    //
    //    template <typename T> struct function_traits;
    //    template <typename R, typename... Args>
    //    struct function_traits<R(Args...)>
    //    {
    //        using signature_t = R(Args...);
    //        using return_t = R;
    //        using args_t = std::tuple<Args...>;
    //    };
    //
    //    template <typename T>
    //    using signature_t = typename function_traits<T>::signature_t;
    //    template <typename T>
    //    using signature_return_t = typename function_traits<T>::return_t;
    //    template <typename T>
    //    using signature_args_t = typename function_traits<T>::args_t;
    //
    //    template <typename T, typename Sig> concept bool Invokable()
    //    {
    //        return requires(T t, signature_args_t<Sig> args)
    //        {
    //            typename signature_t<Sig>;
    //            typename signature_return_t<Sig>;
    //            // clang-format off
    //            {std::apply(t, std::move(args))}
    //            ->signature_return_t<Sig>;
    //            // clang-format on
    //        };
    //    }
    //
    //    //=============================================================================
    //    // utility
    //    //=============================================================================
    //
    //    template <typename T> concept bool Streamable()
    //    {
    //        return requires(std::ostream & os, T t){{os << t}->std::ostream &
    //        };
    //    }
} // namespace csb

#endif // CSB_TYPE_TRAITS_HPP
