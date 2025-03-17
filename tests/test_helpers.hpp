//
// dpw
//
#pragma once

#include <quickkv/quickkv.hpp>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <spdlog/spdlog.h>

namespace helpers {
    constexpr double EPSILON = 1e-5;

    inline std::random_device random_device;  // Obtain a random number from hardware
    inline std::mt19937 generator(random_device());

    inline auto random_int(int min = 1'000'000, int max = 9'999'999) {
        std::uniform_int_distribution<int> distr(min, max);
        return distr(generator);
    }

    inline float random_float(float min = -20.0, float max = 100.0) {
        std::uniform_real_distribution<float> distr(min, max);
        return distr(generator);
    }


    // create a random filename
    inline auto create_temp_path(const Str& name = "tmp-file_") {
        // Get the system's temporary directory
        std::filesystem::path tempDir = std::filesystem::temp_directory_path();

        // Generate a unique filename
        Str filename = name + std::to_string(random_int()) + ".tmp"; // Simple unique filename

        // Create a temporary file path
        FilePath path = tempDir / filename;

        // Create the temporary file
        std::ofstream ofs(path);
        if (!ofs) {
            throw std::runtime_error("Failed to create temporary file");
        }

        // Close the file (optional, as it will be closed when the ofstream goes out of scope)
        ofs.close();

        return path;
    }

    inline auto remove_temp_path(const FilePath& path) {
        try {
            std::filesystem::remove_all(path);
        } catch (const std::exception& e) {
            spdlog::error("Failed to remove temporary file {}", e.what());
        }
    }


    inline std::string capture_stdout(Func<void()> func) {
        std::ostringstream oss;
        std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());  // Redirect std::cout

        func();  // Execute function that prints to stdout

        std::cout.rdbuf(old_cout);  // Restore std::cout
        return oss.str();
    }

    // sample data
    constexpr StrView raw_temps_data = "1740422400.cottage.0=15.343750\n1740422700.cottage.0=15.458330\n1740423000.cottage.0=15.651040\n1740423300.cottage.0=15.739580\n1740423600.cottage.0=16.010420\n1740423900.cottage.0=16.192711\n1740424200.cottage.0=16.244789\n1740424500.cottage.0=16.062500\n1740424800.cottage.0=16.041670\n1740425100.cottage.0=15.833330\n1740425400.cottage.0=15.812500\n1740425700.cottage.0=16.234381";


}
