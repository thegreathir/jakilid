cmake_minimum_required(VERSION 3.4)
project(jakilid)

set(CMAKE_CXX_STANDARD 17)

include_directories(libcuckoo)

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(extern/pybind11/include)
add_subdirectory(extern/pybind11)

pybind11_add_module(jakilid py.cpp)

target_link_libraries(jakilid PRIVATE ${Boost_LIBRARIES})
target_link_libraries(jakilid PRIVATE pthread)
target_link_libraries(jakilid PRIVATE rt)

add_subdirectory(test)
