#ifndef JAKILID_TRAITS_HPP
#define JAKILID_TRAITS_HPP

#include <boost/mpl/bool.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>

#include <type_traits>
#include <string>

namespace jakilid {

namespace traits {

// char

template<class T>
struct __is_char : boost::mpl::false_ {};

template<>
struct __is_char<char> : boost::mpl::true_ {};

template<class T>
struct is_char : __is_char<std::remove_cv_t<T>> {};

// wchar

template<class T>
struct __is_wchar : boost::mpl::false_ {};

template<>
struct __is_wchar<wchar_t> : boost::mpl::true_ {};

template<class T>
struct is_wchar : __is_wchar<std::remove_cv_t<T>> {};

// c string

template <class T>
struct __is_c_string : boost::mpl::false_ {};

template <>
struct __is_c_string<char const*> : boost::mpl::true_ {};

template <>
struct __is_c_string<char*> : boost::mpl::true_ {};

template <std::size_t N>
struct __is_c_string<char[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct __is_c_string<char const[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct __is_c_string<char(&)[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct __is_c_string<char const(&)[N]> : boost::mpl::true_ {};

template<class T>
struct is_c_string : __is_c_string<std::remove_cv_t<T>> {};

// c wstring

template <class T>
struct __is_c_wstring : boost::mpl::false_ {};

template <>
struct __is_c_wstring<wchar_t const*> : boost::mpl::true_ {};

template <>
struct __is_c_wstring<wchar_t*> : boost::mpl::true_ {};

template <std::size_t N>
struct __is_c_wstring<wchar_t[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct __is_c_wstring<wchar_t const[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct __is_c_wstring<wchar_t(&)[N]> : boost::mpl::true_ {};

template <std::size_t N>
struct __is_c_wstring<wchar_t const(&)[N]> : boost::mpl::true_ {};

template<class T>
struct is_c_wstring : __is_c_wstring<std::remove_cv_t<T>> {};

// std string

template<class T>
struct __is_std_string : boost::mpl::false_ {};

template<>
struct __is_std_string<std::string> : boost::mpl::true_ {};

template<class T>
struct is_std_string : __is_std_string<std::remove_cv_t<T>> {};

// std w string

template<class T>
struct __is_std_wstring : boost::mpl::false_ {};

template<>
struct __is_std_wstring<std::wstring> : boost::mpl::true_ {};

template<class T>
struct is_std_wstring : __is_std_wstring<std::remove_cv_t<T>> {};

// bool

template<class T>
struct __is_bool : boost::mpl::false_ {};

template<>
struct __is_bool<bool> : boost::mpl::true_ {};

template<class T>
struct is_bool : __is_bool<std::remove_cv_t<T>> {};

// signed integer number

template<class T>
struct __is_signed_integer : boost::mpl::false_ {};

template<>
struct __is_signed_integer<short> : boost::mpl::true_ {};

template<>
struct __is_signed_integer<int> : boost::mpl::true_ {};

template<>
struct __is_signed_integer<long> : boost::mpl::true_ {};

template<>
struct __is_signed_integer<long long> : boost::mpl::true_ {};

template<class T>
struct is_signed_integer : __is_signed_integer<std::remove_cv_t<T>> {};

// unsigned integer number

template<class T>
struct __is_unsigned_integer : boost::mpl::false_ {};

template<>
struct __is_unsigned_integer<unsigned short> : boost::mpl::true_ {};

template<>
struct __is_unsigned_integer<unsigned int> : boost::mpl::true_ {};

template<>
struct __is_unsigned_integer<unsigned long> : boost::mpl::true_ {};

template<>
struct __is_unsigned_integer<unsigned long long> : boost::mpl::true_ {};

template<class T>
struct is_unsigned_integer : __is_unsigned_integer<std::remove_cv_t<T>> {};

/* ################################################## */

template <class T, class = void> struct __type_char {
    static constexpr char value = '*';
};

template <class T>
struct __type_char<T, std::enable_if_t<__is_signed_integer<T>::value>> {
    static constexpr char value = 'i';
};

template <class T>
struct __type_char<T, std::enable_if_t<__is_unsigned_integer<T>::value>> {
    static constexpr char value = 'u';
};

template <class T>
struct __type_char<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    static constexpr char value = 'f';
};

template <class T>
struct __type_char<T, std::enable_if_t<
        boost::mpl::or_<
            is_c_string<T>,
            is_std_string<T>,
            is_char<T>
        >::value>> {

  static constexpr char value = 's';
};

template <class T>
struct __type_char<T, std::enable_if_t<
        boost::mpl::or_<
            is_c_wstring<T>,
            is_std_wstring<T>,
            is_wchar<T>
        >::value>> {

  static constexpr char value = 'w';
};

template <class T>
struct __type_char<T, std::enable_if_t<is_bool<T>::value>> {
    static constexpr char value = 'b';
};

}

}

#endif