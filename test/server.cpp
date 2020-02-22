#include "../jakilid.hpp"

#include <string>
#include <random>
#include <iostream>

using SharedMap = jakilid::Jakilid<std::string, std::string>;

constexpr auto instance_name = "test_instance";
constexpr size_t number_of_iteration = 1e6;
constexpr auto key_length = 10;
constexpr auto value_length = 20;

auto get_random_string(std::size_t length) {
    std::stringstream str;
    std::generate_n(std::ostream_iterator<char>(str), length, [] {
        static std::random_device rd;
        static std::mt19937 mt{rd()};
        static std::string char_set("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
        static auto dist = std::uniform_int_distribution<std::uint16_t>(0, char_set.size() - 1);

        return char_set[dist(mt)];
    });

    return str.str();
}

int main() {

    auto shared_map = SharedMap::GetInstance(instance_name);

    for (size_t i = 0; i < number_of_iteration; ++i)
        shared_map->Insert(get_random_string(key_length), get_random_string(value_length));

    return 0;
}
