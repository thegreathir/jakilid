#include "jakilid.hpp"

#include <iostream>


namespace jakilid {

template<>
byte_array to_byte_array<std::string>(const std::string &input) {
    return byte_array(input.begin(), input.end());
}

template<>
std::string from_byte_array<std::string>(const byte_array &input) {
    return std::string(reinterpret_cast<const char *>(input.data()), input.size());
}

}
using SharedMap = jakilid::Jakilid<int, std::string>;

int main() {

    auto shared_map = SharedMap::get_instance("map2");

    std::cout << shared_map->find(17) << std::endl;

    return 0;
}
