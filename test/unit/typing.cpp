#include "utils/random.hpp"
#include "utils/mp.hpp"

#include <traits.hpp>
#include <jakilid.hpp>

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <typeinfo>

template<class T>
std::string get_type_char_string() {
    return std::string(jakilid::traits::type_char<T>::value, 2);
}

TEST(typing, test_type_chars) {
    ASSERT_EQ("u0", get_type_char_string<unsigned char>());
    ASSERT_EQ("s0", get_type_char_string<decltype("abcef")>());
    ASSERT_EQ("w0", get_type_char_string<decltype(L"abcef")>());
    ASSERT_EQ("b0", get_type_char_string<decltype(false)>());
    ASSERT_EQ("f1", get_type_char_string<double>());
}

using value_types = std::tuple<
    float,
    char,
    double,
    long,
    unsigned long,
    unsigned short,
    short,
    long long,
    long double,
    std::wstring,
    unsigned int,
    wchar_t,
    std::string,
    unsigned long long,
    int
>;

using pair1 = jakilid::test::construct_pair_tuple<char, value_types>::type;
using pair2 = jakilid::test::construct_pair_tuple<std::string, value_types>::type;
using pair3 = jakilid::test::construct_pair_tuple<int, value_types>::type;

using all_pairs = jakilid::test::concat_tuple<pair1, pair2, pair3>::type;

template<class T>
class TypingFindInsert : public testing::Test {
public:
    using key_type = typename T::first_type;
    using value_type = typename T::second_type;
    using another_type = typename jakilid::test::tuple_another<value_type, value_types>::type;
};


using test_types = jakilid::test::tuple_to_gtest_types<all_pairs, ::testing::Types>::types;

TYPED_TEST_SUITE(TypingFindInsert, test_types);

TYPED_TEST(TypingFindInsert, print) {
    std::cout << typeid(typename TestFixture::key_type).name() << std::endl;
    std::cout << typeid(typename TestFixture::value_type).name() << std::endl;
    std::cout << typeid(typename TestFixture::another_type).name() << std::endl;
}

/*
TEST(sample, sample2) {
    jakilid::SharedDict d("shd");

    d.Insert(-10, L"امیرعباس");
    d.Insert<float>(false, 1.0/4);

    std::wstring res;

    d.Find(-10, res);

    std::cout << (res == L"امیرعباس") << std::endl;
    
    float f;
    d.Find(false, f);

    std::cout << f << std::endl;

    d.Insert("kkk", 'J');

    char cc;
    d.Find("kkk", cc);
    std::cout << cc << std::endl;

    signed char sc = -126;

    d.Insert("SCC", sc);

    signed char gh;
    d.Find("SCC", gh);
    std::cout << (int)gh << std::endl;

}
*/