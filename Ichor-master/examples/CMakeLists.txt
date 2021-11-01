file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/examples/bjorn_usecase/*.cpp)
add_executable(ichor_bjorn_usecase ${EXAMPLE_SOURCES})
target_link_libraries(ichor_bjorn_usecase ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ichor_bjorn_usecase ichor)

file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/examples/scheduler_queue_example/*.cpp)
add_executable(ichor_scheduler_queue_example ${EXAMPLE_SOURCES})
target_link_libraries(ichor_scheduler_queue_example ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ichor_scheduler_queue_example ichor)


file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/examples/yielding_timer_example/*.cpp)
add_executable(ichor_yielding_timer_example ${EXAMPLE_SOURCES})
target_link_libraries(ichor_yielding_timer_example ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ichor_yielding_timer_example ichor)

file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/examples/timer_example/*.cpp)
add_executable(ichor_timer_example ${EXAMPLE_SOURCES})
target_link_libraries(ichor_timer_example ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ichor_timer_example ichor)

file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/examples/multithreaded_example/*.cpp)
add_executable(ichor_multithreaded_example ${EXAMPLE_SOURCES})
target_link_libraries(ichor_multithreaded_example ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ichor_multithreaded_example ichor)

file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/examples/optional_dependency_example/*.cpp)
add_executable(ichor_optional_dependency_example ${EXAMPLE_SOURCES})
target_link_libraries(ichor_optional_dependency_example ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ichor_optional_dependency_example ichor)

if(NOT USE_SANITIZERS)
    file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/examples/realtime_example/gsm_enc/*.cpp)
    file(GLOB_RECURSE EXAMPLE_SOURCES ${TOP_DIR}/examples/realtime_example/*.cpp)
    add_executable(ichor_realtime_example ${EXAMPLE_SOURCES})
    target_link_libraries(ichor_realtime_example ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(ichor_realtime_example ichor)
endif()

