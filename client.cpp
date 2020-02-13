#include "jakilid.hpp"

#include <iostream>

using SharedMap = jakilid::Jakilid<int, std::string>;

int main() {

    auto shared_map = SharedMap::GetInstance("map2");

    std::cout << shared_map->Find(18) << std::endl;

    return 0;
}
