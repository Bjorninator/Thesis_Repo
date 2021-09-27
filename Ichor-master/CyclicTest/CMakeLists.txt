file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/CyclicTest/Cyclic_test_ichor/*.cpp)
add_executable(Cyclic_test_ichor ${EXAMPLE_SOURCES})
target_link_libraries(Cyclic_test_ichor ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Cyclic_test_ichor ichor)

file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/CyclicTest/Cyclic_test_ichor_NP/*.cpp)
add_executable(Cyclic_test_ichor_NP ${EXAMPLE_SOURCES})
target_link_libraries(Cyclic_test_ichor_NP ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Cyclic_test_ichor_NP ichor)

