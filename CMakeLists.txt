cmake_minimum_required(VERSION 3.30)

project(quickkv VERSION 0.3.2 LANGUAGES CXX)
set(BUILD_NUMBER "101")
set (FULL_VERSION "${PROJECT_VERSION}-${BUILD_NUMBER}")
add_definitions(-DPROJECT_VERSION="${FULL_VERSION}")
message(STATUS "Version: ${FULL_VERSION}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download CPM.cmake if not present
set(CPM_DOWNLOAD_VERSION 0.27.2)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT EXISTS ${CPM_DOWNLOAD_LOCATION})
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

# Find required packages
# spdlog
CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.15.0)


# nlohmann_json
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Catch2 3 REQUIRED)

# Enable testing
option(ENABLE_TESTS "Build tests" ON)
if (ENABLE_TESTS)
    enable_testing()

    add_executable(quickkv_tests tests/main.cpp tests/quickkv_tests.cpp)
    target_link_libraries(quickkv_tests PRIVATE 
        quickkv 
        spdlog::spdlog
        Catch2::Catch2WithMain
    )

    add_test(NAME quickkv_tests COMMAND test_quickkv)
endif()


install(DIRECTORY include/ DESTINATION include)


# Export the library for other CMake projects
#install(EXPORT quickkvTargets
#    FILE quickkvTargets.cmake
#    NAMESPACE quickkv::
#    DESTINATION lib/cmake/quickkv
#)

# Define the static library
add_library(quickkv STATIC
    src/quickkv.cpp
)

target_link_libraries(quickkv PUBLIC 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

target_include_directories(quickkv PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/quickkvConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Installation rules
install(TARGETS quickkv
    EXPORT quickkvTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/quickkvConfigVersion.cmake"
    DESTINATION lib/cmake/quickkv
)


