cmake_minimum_required(VERSION 3.8)
project(jakilid-test)

add_executable(client client.cpp)

target_link_libraries(client ${Boost_LIBRARIES})
target_link_libraries(client pthread)
target_link_libraries(client rt)

configure_file(functionality.py ${CMAKE_CURRENT_BINARY_DIR}/functionality.py COPYONLY)

enable_testing()

add_test (NAME functionality
  COMMAND python3 functionality.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_target(test COMMAND ${CMAKE_CTEST_COMMAND})
