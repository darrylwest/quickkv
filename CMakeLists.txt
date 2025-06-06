cmake_minimum_required(VERSION 3.30)

project(quickkv VERSION 0.3.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Download CPM.cmake if not present
set(CPM_DOWNLOAD_VERSION 0.27.2)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT EXISTS ${CPM_DOWNLOAD_LOCATION})
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

# MacOS universal binary setup
if(APPLE)
    execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE HOST_ARCHITECTURE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_ARCHITECTURES "${HOST_ARCHITECTURE}")
    message(STATUS "osx arch: ${CMAKE_OSX_ARCHITECTURES}")
elseif(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    message(STATUS "linux")
endif()

include(${CPM_DOWNLOAD_LOCATION})

# Find required packages
CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.15.0
)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Catch2 3 REQUIRED)

# Enable testing
option(ENABLE_TESTS "Build tests" ON)
if (ENABLE_TESTS)
    enable_testing()
    add_executable(unit_tests
            tests/main.cpp
            tests/quickkv_tests.cpp
    )
    target_link_libraries(unit_tests PRIVATE
            quickkv
            spdlog::spdlog
            domainkeys
            Catch2::Catch2WithMain
    )
    add_test(NAME unit_tests COMMAND unit_tests)
endif()

# Define the static library
add_library(quickkv STATIC
    src/quickkv.cpp
)

target_link_libraries(quickkv PUBLIC
    nlohmann_json::nlohmann_json
    domainkeys
    spdlog::spdlog
)

# Enable position-independent code for security and portability
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

target_include_directories(quickkv PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add examples
file(GLOB EXAMPLE_SOURCES examples/*.cpp)

foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})

    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    target_link_libraries(${EXAMPLE_NAME} PRIVATE quickkv)
endforeach()

# Installation rules
install(DIRECTORY include/ DESTINATION include)
install(TARGETS quickkv
    EXPORT quickkvTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Export the library for other CMake projects
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


