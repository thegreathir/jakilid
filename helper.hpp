#ifndef JAKILID_HELPER_HPP
#define JAKILID_HELPER_HPP

#include<boost/mpl/bool.hpp>
#include<boost/mpl/and.hpp>
#include<boost/mpl/not.hpp>

#include<string>
#include <type_traits>

namespace jakilid {

namespace helper {

// C string

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

// STD string

template <typename T>
struct is_std_string : boost::mpl::false_ {};

template <typename T>
struct is_std_string<T const> : is_std_string<T> {};

template <typename T, typename Traits, typename Allocator>
struct is_std_string<std::basic_string<T, Traits, Allocator> > : boost::mpl::true_ {};

// bool

template <typename T>
struct is_bool : boost::mpl::false_ {};

template <typename T>
struct is_bool<T const> : is_bool<T> {};

template <>
struct is_bool<bool> : boost::mpl::true_ {};

// char

template <typename T>
struct is_char : boost::mpl::false_ {};

template <typename T>
struct is_char<T const> : is_char<T> {};

template <>
struct is_char<char> : boost::mpl::true_ {};

template <>
struct is_char<wchar_t> : boost::mpl::true_ {};

// integer

template<typename T>
struct is_numerical_integral :
    boost::mpl::and_<std::is_integral<T>, boost::mpl::not_<is_char<T>>> {};

}

}

#endif