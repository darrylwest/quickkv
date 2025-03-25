//
// 2025-03-24 17:01:53 dpw
//

#pragma once

#include <array>
#include <ctime>
#include <stdexcept>
#include <string>

namespace domainkeys::keys {
    constexpr std::string_view VERSION = "0.5.2-106";
    constexpr size_t TXKEY_SIZE = 12;
    constexpr size_t RTKEY_SIZE = 16;

    // timestamp key with microsecond timestamp + 4 byte random bytes converted to base62
    struct TimestampKey {
        std::array<char, TXKEY_SIZE> data{};
        [[nodiscard]] size_t size() const { return data.size(); };

        // construct from a string of exactly 12 chars
        explicit TimestampKey(const std::string &str) {
            if (str.size() != TXKEY_SIZE) {
                throw std::runtime_error("invalid timestamp key size");
            }
            std::copy(str.begin(), str.end(), data.begin());
        }

        [[nodiscard]] constexpr auto begin() const { return data.begin(); }

        [[nodiscard]] constexpr auto end() const { return data.begin() + TXKEY_SIZE; }

        [[nodiscard]] std::string to_string() const { return {data.data(), TXKEY_SIZE}; };
    };

    // routing key with 4 byte route, microsecond timestamp + 4 byte random bytes converted to base62
    struct RouteKey {
        std::array<char, RTKEY_SIZE> data{};
        [[nodiscard]] size_t size() const { return data.size(); };

        // construct from a string of exactly 16 chars
        explicit RouteKey(const std::string &str) {
            if (str.size() != RTKEY_SIZE) {
                throw std::runtime_error("invalid timestamp key size");
            }
            std::copy(str.begin(), str.end(), data.begin());
        }

        [[nodiscard]] constexpr auto begin() const { return data.begin(); }

        [[nodiscard]] constexpr auto end() const { return data.begin() + TXKEY_SIZE; }

        [[nodiscard]] std::string to_string() const { return {data.data(), RTKEY_SIZE}; };
    };

    // create a new timestamp key
    TimestampKey create_timestamp_key();

    // create the route key
    RouteKey create_route_key(const std::string &route = "");

    // create a ramdom base62 key of a specified length
    std::string create_random_key(size_t size = 16);

    // return current timestamp for now in microseconds
    std::time_t now_microseconds();
} // namespace domainkeys::keys
