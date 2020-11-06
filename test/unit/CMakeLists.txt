
find_package(GTest)

aux_source_directory(. TEST_FILES)
foreach(TEST_FILE ${TEST_FILES})
    string(REGEX MATCH "^\\./(.*)\\.[^.]*$" DUMMY ${TEST_FILE})

    add_executable(${CMAKE_MATCH_1} "${TEST_FILE}")
    target_link_libraries(${CMAKE_MATCH_1} gtest_main gtest)
    target_link_libraries(${CMAKE_MATCH_1} ${Boost_LIBRARIES})
    target_link_libraries(${CMAKE_MATCH_1} pthread)
    target_link_libraries(${CMAKE_MATCH_1} rt)

    gtest_discover_tests(${CMAKE_MATCH_1})
endforeach()