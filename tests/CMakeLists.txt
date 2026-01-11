project(Tests)

add_executable(test_suite
    test_main.cpp
    test_entities.cpp
    test_managers.cpp
    test_physics.cpp
)

target_link_libraries(test_suite
    PRIVATE
    core_lib
    Boost::unit_test_framework
    raylib
)

# Register tests
add_test(NAME AllTests COMMAND test_suite)
