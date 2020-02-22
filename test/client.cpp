#include "../jakilid.hpp"

#include <iostream>
#include <string>

constexpr auto instance_name = "test_instance";

using SharedMap = jakilid::Jakilid<std::string, std::string>;

int main() {

    auto shared_map = SharedMap::GetInstance(instance_name);
    std::cout << shared_map->Size() << std::endl;

    return 0;
}
