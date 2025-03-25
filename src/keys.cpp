//
// 2025-03-24 17:01:53 dpw
//
#include <chrono>
#include <domainkeys/base62.hpp>
#include <domainkeys/keys.hpp>
#include <print>
#include <random>

namespace domainkeys::keys {
    // the min/max keeps the base62 encodeing to 3 chars 100..zzz
    constexpr int BASE62_MIN = 3844;
    constexpr int BASE62_MAX = 238327;

    inline std::random_device random_device; // Obtain a random number from hardware
    inline std::mt19937 generator(random_device());

    inline auto random_int(int min = BASE62_MIN, int max = BASE62_MAX) {
        std::uniform_int_distribution<int> distr(min, max);
        return distr(generator);
    }

    std::time_t now_microseconds() {
        using namespace std::chrono;
        auto now = system_clock::now();
        return duration_cast<microseconds>(now.time_since_epoch()).count();
    }
    // TODO get now microseconds
    // TODO generate random int

    // TODO encode to base62
    // TODO decode base62 to int

    // create a new timestamp key
    TimestampKey create_timestamp_key() {
        auto now = now_microseconds();
        auto rand = random_int();

        auto ss = base62::encodeBase62(now) + base62::encodeBase62(rand);

        return TimestampKey{ss};
    }

    // create the route key
    RouteKey create_route_key(const std::string &route) {
        std::string rt = route;
        while (rt.size() < 4) {
            int n = random_int(0, 61);
            rt.append(1, base62::base62_chars[n]);
        }

        auto tx = create_timestamp_key().to_string();
        return RouteKey(rt + tx);
    }

    // create a random key the lenth specified by size
    std::string create_random_key(size_t size) {
        std::string key;
        key.reserve(size);
        for (size_t i = 0; i < size; i++) {
            key.push_back(base62::base62_chars[random_int(0, 61)]);
        }
        return key;
    }
} // namespace domainkeys::keys
