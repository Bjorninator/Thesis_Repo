file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/UseCase/use-case/*.cpp)
add_executable(usecase_ichor ${EXAMPLE_SOURCES})
target_link_libraries(usecase_ichor ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(usecase_ichor ichor)


