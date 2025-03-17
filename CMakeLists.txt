cmake_minimum_required(VERSION 3.30)

project(quickkv VERSION 0.3.2 LANGUAGES CXX)
set(BUILD_NUMBER "101")
set (FULL_VERSION "${PROJECT_VERSION}-${BUILD_NUMBER}")
add_definitions(-DPROJECT_VERSION="${FULL_VERSION}")
message(STATUS "Version: ${FULL_VERSION}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the static library
add_library(quickkv STATIC
    src/quickkv.cpp
)

# spdlog
find_package(spdlog 1.15.0 REQUIRED)
target_link_libraries(quickkv PUBLIC spdlog::spdlog)

# nlohmann_json
find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(quickkv PUBLIC nlohmann_json::nlohmann_json)

target_include_directories(quickkv PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Enable testing
option(ENABLE_TESTS "Build tests" ON)
if (ENABLE_TESTS)
    enable_testing()

    # Catch2
    # unit test lib catch2
    Include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(quickkv_tests tests/main.cpp tests/quickkv_tests.cpp)
    target_link_libraries(quickkv_tests PRIVATE quickkv Catch2::Catch2WithMain)

    add_test(NAME quickkv_tests COMMAND test_quickkv)
endif()


# Installation rules
install(TARGETS quickkv
    EXPORT quickkvTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

# Export the library for other CMake projects
install(EXPORT quickkvTargets
    FILE quickkvTargets.cmake
    NAMESPACE quickkv::
    DESTINATION lib/cmake/quickkv
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/quickkvConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/quickkvConfigVersion.cmake"
    DESTINATION lib/cmake/quickkv
)


