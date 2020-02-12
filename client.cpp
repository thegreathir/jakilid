#include "jakilid.hpp"

#include <iostream>

using SharedMap = jakilid::Jakilid<int, std::string>;

int main() {

    auto shared_map = SharedMap::get_instance("map2");

    std::cout << shared_map->find(17) << std::endl;

    return 0;
}
