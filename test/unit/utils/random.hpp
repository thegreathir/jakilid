#ifndef JAKILID_TEST_RANDOM_HPP
#define JAKILID_TEST_RANDOM_HPP

#include <traits.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/or.hpp>

#include <algorithm>
#include <limits>
#include <random>
#include <string>
#include <sstream>
#include <iterator>
#include <type_traits>
#include <iostream>

namespace jakilid {

namespace test {

class Random {
public:
    Random()
    : rd()
    , mt(rd())
    {}

    using integers = boost::mpl::vector<
        short,
        unsigned short,
        int,
        unsigned int,
        long,
        unsigned long,
        long long,
        unsigned long long
    >;

    template<class T>
    std::enable_if_t<boost::mpl::contains<integers, T>::value, T>
    generate() {
        return std::uniform_int_distribution<T>(
            std::numeric_limits<T>::min(), std::numeric_limits<T>::max()
        )(mt);
    }

    template<class T>
    std::enable_if_t<std::is_floating_point_v<T>, T>
    generate() {
        return std::uniform_real_distribution<T>(
            std::numeric_limits<T>::min(), std::numeric_limits<T>::max()
        )(mt);
    }

    template<class T>
    std::enable_if_t<traits::is_std_string<T>::value, T>
    generate() {
        return generate_string(char_set, get_random_length());
    }

    template<class T>
    std::enable_if_t<traits::is_std_wstring<T>::value, T>
    generate() {
        return generate_string(wchar_set, get_random_length());
    }

    template<class T>
    std::enable_if_t<traits::is_char<T>::value, T>
    generate() {
        return generate_string(char_set, 1)[0];
    }

    template<class T>
    std::enable_if_t<traits::is_wchar<T>::value, T>
    generate() {
        return generate_string(wchar_set, 1)[0];
    }


private:

    size_t get_random_length() {
        constexpr size_t max_len = 1000;
        constexpr size_t min_len = 10;

        return std::uniform_int_distribution<size_t>(min_len, max_len)(mt);
    }

    template<class CharT>
    std::basic_string<CharT> generate_string(
            const std::basic_string<CharT>& char_set, size_t length) {

        std::basic_stringstream<CharT> str;
        std::generate_n(std::ostream_iterator<CharT, CharT>(str), length, [this, char_set, length] {
            auto dist = std::uniform_int_distribution<std::uint16_t>(0, char_set.size() - 1);
            return char_set[dist(mt)];
        });

        return str.str();
    }

    std::random_device rd;
    std::mt19937 mt;
    static const inline std::string char_set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const inline std::wstring wchar_set = L"ابپتثجچحخدذرزژسشصضطظعغفقمکگلمنوهی۰۱۲۳۴۵۶۷۸۹豆贝尔维艾丝艾娜艾勒";
};

}

}

#endif