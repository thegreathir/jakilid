cmake_minimum_required(VERSION 3.8)
project(jakilid-test)

add_executable(insert server.cpp)
add_executable(find client.cpp)

target_link_libraries(insert ${Boost_LIBRARIES})
target_link_libraries(insert pthread)
target_link_libraries(insert rt)

target_link_libraries(find ${Boost_LIBRARIES})
target_link_libraries(find pthread)
target_link_libraries(find rt)
