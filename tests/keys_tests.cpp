//
// 2025-03-24 17:01:53 dpw
//

#include <domainkeys/keys.hpp>
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <print>
#include <chrono>
#include <iomanip>
#include <set>

auto date_to_timestamp(const int year, const int month = 12, const int day = 31) {
    std::tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    std::time_t tt_value = mktime(&tm);
    // Create a time_point from time_t
    auto time_point = std::chrono::system_clock::from_time_t(tt_value);

    // Convert time_point to microseconds since epoch
    auto duration = time_point.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

TEST_CASE("Library Version", "[version]") {
    REQUIRE(domainkeys::keys::VERSION >= "0.5.2-105");
    std::string version = std::string(domainkeys::keys::VERSION);
    REQUIRE(version.size() >= 8);
    REQUIRE(version.starts_with("0.5"));
}

TEST_CASE("TimestampKeys constructed", "[keys][txkey]") {
    domainkeys::keys::TimestampKey key("123456789012");
    REQUIRE(key.size() == domainkeys::keys::TXKEY_SIZE);
}

TEST_CASE("TimestampKeys constructed", "[keys][bad-size]") {
    INFO("test the size of the timestamp key with a bad size; should throw");

    try {
        domainkeys::keys::TimestampKey("12345");
        REQUIRE(false);
    } catch (std::runtime_error& e) {
        REQUIRE(true);
    }
}

TEST_CASE("RouteKeys can be constructed", "[keys][rtkey]") {
    domainkeys::keys::RouteKey key("1234567890123456");
    REQUIRE(key.size() == domainkeys::keys::RTKEY_SIZE);
}

TEST_CASE("RouteKeys constructed", "[keys][bad-size]") {
    INFO("test the size of the route key with a bad size; should throw");

    try {
        domainkeys::keys::RouteKey("TOO-TOO-BIG-TO-BE-VALID");
        REQUIRE(false);
    } catch (std::runtime_error& e) {
        REQUIRE(true);
    }
}

TEST_CASE("TimstampKeys min/max", "[keys][txkey-min-max]") {
    int year = 2020;
    auto past_date = date_to_timestamp(year, 1, 1);
    // std::println("past: {}-{}-{} {}", year, 12, 31, past_date);

    auto sdt = std::to_string(past_date);

    INFO("test the string length of past date to ensure it's 16 chars");
    REQUIRE(sdt.size() == 16);

    year = 2300;
    auto future_date = date_to_timestamp(year, 12, 31);
    // std::println("future: {}-{}-{} ts: {}", year, 12, 31, future_date);

    sdt = std::to_string(future_date);

    INFO("test the string length of the future date to ensure it's 17 chars");
    REQUIRE(sdt.size() == 17);
}

TEST_CASE("TimestapKeys create", "[keys][txkey-create]") {
    const size_t count = 1000;
    std::set<std::string> keys;

    for (int i = 0; i < count; i++) {
        auto key = domainkeys::keys::create_timestamp_key();
        // std::println("key: {}", key.to_string());
        REQUIRE(key.size() == domainkeys::keys::TXKEY_SIZE);
        keys.insert(key.to_string());
    }

    INFO("test the number of txkeys created, should equal the count to ensure uniqueness");
    REQUIRE(keys.size() == count);
}

TEST_CASE("RouteKey create", "[keys][rtkey-create-no-route]") {
    const size_t count = 1000;
    std::set<std::string> keys;

    for (int i = 0; i < count; i++) {
        auto key = domainkeys::keys::create_route_key();
        // std::println("key: {}", key.to_string());
        REQUIRE(key.size() == domainkeys::keys::RTKEY_SIZE);
        keys.insert(key.to_string());
    }

    INFO("test the number of rtkeys created, should equal the count to ensure uniqueness");
    REQUIRE(keys.size() == count);
}

TEST_CASE("RouteKey create", "[keys][rtkey-create-with-route]") {
    const size_t count = 1000;
    std::set<std::string> keys;
    std::string route = "test";

    for (int i = 0; i < count; i++) {
        auto key = domainkeys::keys::create_route_key(route);
        // std::println("key: {}", key.to_string());
        REQUIRE(key.size() == domainkeys::keys::RTKEY_SIZE);
        keys.insert(key.to_string());
        REQUIRE(key.to_string().starts_with(route));
    }

    INFO("test the number of routed rtkeys created, should equal the count to ensure uniqueness");
    REQUIRE(keys.size() == count);
}

TEST_CASE("RouteKey create", "[keys][rtkey-partial-route]") {
    const size_t count = 1000;
    std::set<std::string> keys;
    std::string route = "01";

    for (int i = 0; i < count; i++) {
        auto key = domainkeys::keys::create_route_key(route);
        // std::println("key: {}", key.to_string());
        REQUIRE(key.size() == domainkeys::keys::RTKEY_SIZE);
        REQUIRE(key.to_string().starts_with(route));
        keys.insert(key.to_string());
    }

    INFO("test the number of routed rtkeys created, should equal the count to ensure uniqueness");
    REQUIRE(keys.size() == count);
}

TEST_CASE("RandomKey create", "[keys][ramdom-create]") {
    std::string key = domainkeys::keys::create_random_key();
    INFO("test the size of the random key with default size of 16");
    REQUIRE(key.size() == 16);

    size_t size = 8;
    key = domainkeys::keys::create_random_key(size);
    INFO("test the size of the random key with specified size of 8");
    REQUIRE(key.size() == size);
}

TEST_CASE("RandomKey create", "[keys][random-unique]") {
    const size_t count = 1000;
    std::set<std::string> keys;
    size_t size = 12;

    for (int i = 0; i < count; i++) {
        auto key = domainkeys::keys::create_random_key(size);
        // std::println("key: {}", key.to_string());
        REQUIRE(key.size() == size);
        keys.insert(key);
    }

    INFO("test the number of random keys created, should equal the count to ensure uniqueness");
    REQUIRE(keys.size() == count);
}