cmake_minimum_required(VERSION 3.8)
project(jakilid)

set(CMAKE_CXX_STANDARD 17)

include_directories("libcuckoo")

find_package(Boost)
include_directories("${Boost_INCLUDE_DIRS}")

add_subdirectory(test)
