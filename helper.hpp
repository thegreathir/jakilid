#ifndef JAKILID_HELPER_HPP
#define JAKILID_HELPER_HPP

#include<boost/mpl/bool.hpp>

#include<string>

namespace jakilid {

namespace helper {

template <typename T>
struct is_c_string : boost::mpl::false_ {};

template <typename T>
struct is_c_string<T const> : is_c_string<T> {};

template <>
struct is_c_string<char const*> : boost::mpl::true_ {};

template <>
struct is_c_string<wchar_t const*> : boost::mpl::true_ {};

template <>
struct is_c_string<char*> : boost::mpl::true_ {};

template <>
struct is_c_string<wchar_t*> : boost::mpl::true_ {};

template <std::size_t N>
struct is_c_string<char[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct is_c_string<wchar_t[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct is_c_string<char const[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct is_c_string<wchar_t const[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct is_c_string<char(&)[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct is_c_string<wchar_t(&)[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct is_c_string<char const(&)[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct is_c_string<wchar_t const(&)[N]> : boost::mpl::true_ {};

template <typename T>
struct is_std_string : boost::mpl::false_ {};

template <typename T>
struct is_std_string<T const> : is_std_string<T> {};

template <typename T, typename Traits, typename Allocator>
struct is_std_string<std::basic_string<T, Traits, Allocator> > : boost::mpl::true_ {};

template <typename T>
struct is_bool : boost::mpl::false_ {};

template <typename T>
struct is_bool<T const> : is_bool<T> {};

template <>
struct is_bool<bool> : boost::mpl::true_ {};

}

}

#endif