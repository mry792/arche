###
# Main library
###

add_library(${PROJECT_NAME} INTERFACE)
file(GLOB_RECURSE source_files CONFIGURE_DEPENDS "*.hpp")
target_sources(${PROJECT_NAME} INTERFACE ${source_files})
target_include_directories(${PROJECT_NAME} INTERFACE ".")


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)

    if(BUILD_TESTING)
        find_package(Catch2 REQUIRED)
        include(Catch)

        find_package(trompeloeil REQUIRED)

        file(GLOB_RECURSE test_files CONFIGURE_DEPENDS "*.test.cpp")

        ###
        # Unit tests
        ###

        add_executable(unit_test
            unit_test_main.cpp
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
            trompeloeil::trompeloeil

            # local
            ${PROJECT_NAME}
        )

        catch_discover_tests(unit_test)
    endif()
endif()
