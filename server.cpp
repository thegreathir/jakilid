#include "jakilid.hpp"

using SharedMap = jakilid::Jakilid<int, std::string>;

int main() {

    auto shared_map = SharedMap::get_instance("map2");

    shared_map->insert(17, "amirabbas");
    return 0;
}
