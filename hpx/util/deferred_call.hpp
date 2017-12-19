//  Copyright (c) 2014-2015 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// hpxinspect:nodeprecatedname:is_callable
// hpxinspect:nodeprecatedname:util::result_of

#ifndef HPX_UTIL_DEFERRED_CALL_HPP
#define HPX_UTIL_DEFERRED_CALL_HPP

#include <hpx/config.hpp>
#include <hpx/traits/get_function_address.hpp>
#include <hpx/traits/get_function_annotation.hpp>
#include <hpx/traits/is_callable.hpp>
#include <hpx/util/decay.hpp>
#include <hpx/util/invoke_fused.hpp>
#include <hpx/util/result_of.hpp>
#include <hpx/util/tuple.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace traits { namespace detail
{
    template <typename T>
    struct is_deferred_callable;

    template <typename F, typename ...Ts>
    struct is_deferred_callable<F(Ts...)>
      : is_callable<
            typename util::decay_unwrap<F>::type(
                typename util::decay_unwrap<Ts>::type...)
        >
    {};

    template <typename F, typename ...Ts>
    struct is_deferred_invocable
      : is_invocable<
            typename util::decay_unwrap<F>::type,
            typename util::decay_unwrap<Ts>::type...
        >
    {};

}}}

namespace hpx { namespace util
{
    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template <typename T>
        struct deferred_result_of;

        template <typename F, typename ...Ts>
        struct deferred_result_of<F(Ts...)>
          : util::result_of<
                typename util::decay_unwrap<F>::type(
                    typename util::decay_unwrap<Ts>::type...)
            >
        {};

        template <typename F, typename ...Ts>
        struct invoke_deferred_result
          : util::invoke_result<
                typename util::decay_unwrap<F>::type,
                typename util::decay_unwrap<Ts>::type...
            >
        {};

        ///////////////////////////////////////////////////////////////////////
        template <typename F, typename ...Ts>
        class deferred;

        template <typename F, typename ...Ts>
        class deferred
        {
        public:
            deferred() {} // needed for serialization

            template <typename F_, typename ...Ts_, typename =
                typename std::enable_if<
                    !std::is_same<typename std::decay<F_>::type, deferred>::value
                >::type>
            explicit HPX_HOST_DEVICE deferred(F_&& f, Ts_&&... vs)
              : _f(std::forward<F_>(f))
              , _args(std::forward<Ts_>(vs)...)
            {}

#if !defined(__NVCC__) && !defined(__CUDACC__)
            deferred(deferred&&) = default;
#else
            HPX_HOST_DEVICE deferred(deferred&& other)
              : _f(std::move(other._f))
              , _args(std::move(other._args))
            {}
#endif

            deferred(deferred const&) = delete;
            deferred& operator=(deferred const&) = delete;

            HPX_HOST_DEVICE HPX_FORCEINLINE
            typename invoke_deferred_result<F, Ts...>::type
            operator()()
            {
                return util::invoke_fused(std::move(_f), std::move(_args));
            }

            template <typename Archive>
            void serialize(Archive& ar, unsigned int const /*version*/)
            {
                ar & _f;
                ar & _args;
            }

            std::size_t get_function_address() const
            {
                return traits::get_function_address<
                        typename util::decay_unwrap<F>::type
                    >::call(_f);
            }

            char const* get_function_annotation() const
            {
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
                return traits::get_function_annotation<
                        typename util::decay_unwrap<F>::type
                    >::call(_f);
#else
                return nullptr;
#endif
            }

#if HPX_HAVE_ITTNOTIFY != 0 && !defined(HPX_HAVE_APEX)
            util::itt::string_handle get_function_annotation_itt() const
            {
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
                return traits::get_function_annotation_itt<
                        typename util::decay_unwrap<F>::type
                    >::call(_f);
#else
                static util::itt::string_handle sh("deferred");
                return sh;
#endif
            }
#endif

        private:
            typename util::decay_unwrap<F>::type _f;
            util::tuple<typename util::decay_unwrap<Ts>::type...> _args;
        };
    }

    template <typename F, typename ...Ts>
    detail::deferred<
        typename std::decay<F>::type,
        typename std::decay<Ts>::type...>
    deferred_call(F&& f, Ts&&... vs)
    {
        static_assert(
            traits::detail::is_deferred_callable<F&&(Ts&&...)>::value
          , "F shall be Callable with decay_t<Ts> arguments");

        typedef detail::deferred<
            typename std::decay<F>::type,
            typename std::decay<Ts>::type...
        > result_type;

        return result_type(std::forward<F>(f), std::forward<Ts>(vs)...);
    }

    // nullary functions do not need to be bound again
    template <typename F>
    inline typename std::decay<F>::type
    deferred_call(F&& f)
    {
        static_assert(
            traits::detail::is_deferred_callable<F&&()>::value
          , "F shall be Callable with no arguments");

        return std::forward<F>(f);
    }
}}

#if defined(HPX_HAVE_THREAD_DESCRIPTION)
///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename F, typename ...Ts>
    struct get_function_address<util::detail::deferred<F, Ts...> >
    {
        static std::size_t
            call(util::detail::deferred<F, Ts...> const& f) noexcept
        {
            return f.get_function_address();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename F, typename ...Ts>
    struct get_function_annotation<util::detail::deferred<F, Ts...> >
    {
        static char const*
            call(util::detail::deferred<F, Ts...> const& f) noexcept
        {
            return f.get_function_annotation();
        }
    };

#if HPX_HAVE_ITTNOTIFY != 0 && !defined(HPX_HAVE_APEX)
    template <typename F, typename ...Ts>
    struct get_function_annotation_itt<util::detail::deferred<F, Ts...> >
    {
        static util::itt::string_handle
            call(util::detail::deferred<F, Ts...> const& f) noexcept
        {
            return f.get_function_annotation_itt();
        }
    };
#endif
}}
#endif

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace serialization
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Archive, typename F, typename ...Ts>
    HPX_FORCEINLINE
    void serialize(
        Archive& ar
      , ::hpx::util::detail::deferred<F, Ts...>& d
      , unsigned int const version = 0
    )
    {
        d.serialize(ar, version);
    }
}}

#endif
