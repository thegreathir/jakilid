#include "utils/random.hpp"

#include <traits.hpp>
#include <jakilid.hpp>
#include <mp.hpp>

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

TEST(typing, test_char_string_cast) {
    jakilid::SharedDict dict{"__test__chstr"};
    jakilid::test::Random rnd;

    const auto key1 = rnd.generate<std::string>();
    const auto key2 = rnd.generate<int>();

    const auto valstr = rnd.generate<std::string>();
    const auto valchr = rnd.generate<char>();


    EXPECT_TRUE(dict.Insert(key1, valstr));

    char read_chr;
    EXPECT_TRUE(dict.Find(key1, read_chr));
    EXPECT_EQ(valstr[0], read_chr);


    EXPECT_TRUE(dict.Insert(key2, valchr));

    std::string read_str;
    EXPECT_TRUE(dict.Find(key2, read_str));
    EXPECT_EQ(valchr, read_str[0]);

    long long another_val;
    EXPECT_THROW({
        dict.Find(key2, another_val);
    }, std::domain_error);
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

using pair1 = jakilid::mp::construct_pair_tuple<char, value_types>::type;
using pair2 = jakilid::mp::construct_pair_tuple<std::string, value_types>::type;
using pair3 = jakilid::mp::construct_pair_tuple<int, value_types>::type;

using all_pairs = jakilid::mp::concat_tuple<pair1, pair2, pair3>::type;

template<class T>
class TypingFindInsert : public testing::Test {
public:
    using key_type = typename T::first_type;
    using value_type = typename T::second_type;
    using another_type = typename jakilid::mp::tuple_another<value_type, value_types>::type;

    jakilid::SharedDict dict;
    jakilid::test::Random rnd_engine;

    TypingFindInsert()
    : dict("__test__shd")
    {
        dict.Drop();
    };
};


using test_types = jakilid::mp::tuple_to_gtest_types<all_pairs, ::testing::Types>::types;

TYPED_TEST_SUITE(TypingFindInsert, test_types);

TYPED_TEST(TypingFindInsert, test_typing_functionality) {

    const auto key1 = this->rnd_engine.template generate<typename TestFixture::key_type>();

    typename TestFixture::value_type val;
    EXPECT_FALSE(this->dict.Find(key1, val));
    
    const auto val1 = this->rnd_engine.template generate<typename TestFixture::value_type>();
    EXPECT_TRUE(this->dict.Insert(key1, val1));

    typename TestFixture::value_type val2;
    EXPECT_TRUE(this->dict.Find(key1, val2));
    EXPECT_EQ(val1, val2);

    typename TestFixture::another_type another_val;
    EXPECT_THROW({
        this->dict.Find(key1, another_val);
    }, std::domain_error);
}
