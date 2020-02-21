#include "../jakilid.hpp"

#include <string>

using SharedMap = jakilid::Jakilid<int, std::string>;

int main() {

    auto shared_map = SharedMap::GetInstance("map2");

    shared_map->Insert(18, "amirabbas");
    return 0;
}
