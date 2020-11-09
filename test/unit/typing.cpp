#include <gtest/gtest.h>
#include <jakilid.hpp>

#include <iostream>

TEST(sample, sample1) {
    unsigned short x = 17;
    long long y = static_cast<long long>(x);
    short z =static_cast<short>(y);
    std::cout << z << std::endl;
    std::cout << sizeof(x) << std::endl;
    std::cout << sizeof(y) << std::endl;
    std::cout << sizeof(z) << std::endl;
}

TEST(sample, sample2) {
    jakilid::SharedDict d("shd");

    d.Insert(-10, L"امیرعباس");
    d.Insert(false, 1.0/4);

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