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