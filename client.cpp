#include "jakilid.hpp"

#include <iostream>

using SharedMap = jakilid::Jakilid<int, int>;

int main() {

    auto shared_map = SharedMap::get_instance("map1");

    std::cout << shared_map->find(17) << std::endl;

    return 0;
}
