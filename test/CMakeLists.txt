cmake_minimum_required(VERSION 3.8)
project(jakilid-test)

add_executable(client client.cpp)

target_link_libraries(client ${Boost_LIBRARIES})
target_link_libraries(client pthread)
target_link_libraries(client rt)
