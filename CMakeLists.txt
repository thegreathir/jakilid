cmake_minimum_required(VERSION 3.8)
project(jakilid)

set(CMAKE_CXX_STANDARD 14)

include_directories("libcuckoo")

find_package(Boost)
include_directories("${Boost_INCLUDE_DIRS}")

add_executable(server server.cpp)
add_executable(client client.cpp jakilid.hpp)

target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server pthread)
target_link_libraries(server rt)

target_link_libraries(client ${Boost_LIBRARIES})
target_link_libraries(client pthread)
target_link_libraries(client rt)
