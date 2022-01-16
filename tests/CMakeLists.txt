
find_package(Catch2 REQUIRED)
include(Catch)

file(GLOB_RECURSE test_files CONFIGURE_DEPENDS "test_*.cpp")

add_executable(unit_test
    main.cpp
    ${test_files}
)
target_compile_options(unit_test
    PRIVATE
        -Werror
        -Wall
        -Wextra
)
target_link_libraries(unit_test
    # 3rd-party
    Catch2::Catch2

    # local
    ${PROJECT_NAME}
)

catch_discover_tests(unit_test)
