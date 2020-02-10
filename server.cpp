#include "jakilid.hpp"

using SharedMap = jakilid::Jakilid<int, int>;

int main() {

    auto shared_map = SharedMap::get_instance("map1");

    shared_map->insert(17, 24);

    return 0;
}
