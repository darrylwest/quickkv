//
// Created by dpw on 2/27/25.
//
#include <quickkv/quickkv.hpp>
#include <domainkeys/keys.hpp>
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <filesystem>
#include <print>

#include "test_helpers.hpp"

void populate_database(quickkv::KVStore& store, const size_t size = 100) {

    for (size_t i = 0; i < size; ++i) {
        auto key = domainkeys::keys::create_route_key().to_string();
        auto value = helpers::random_float(20.0, 30.0);

        store.set(key, std::to_string(value));
    }
}

TEST_CASE("Quick KV Tests", "[quickkv][version]") {

    auto vers = quickkv::get_version();
    std::println("Version: {}", vers);

    REQUIRE(vers.starts_with("0.4."));
}

TEST_CASE("Quick KV Tests", "[quickkv][set_get]") {
    quickkv::KVStore store;
    REQUIRE(store.size() == 0);
    size_t size = 10;
    populate_database(store, size);
    REQUIRE(store.size() == size);

    auto keys = store.keys();

    for (const auto& key : keys) {
        INFO("find for key: " << key);
        auto ret = store.get(key);
        if (ret) {
            REQUIRE(true);
        } else {
            REQUIRE(false);
            continue;
        }

        const auto value = *ret;

        auto new_value = helpers::random_float(30.0, 40.0);
        auto resp = store.set(key, std::to_string(new_value));
        INFO("replacing/updating a value should always return false");
        REQUIRE(resp == false);
        ret = store.get(key);
        if (ret) {
            REQUIRE(true);
        } else {
            REQUIRE(false);
            continue;
        }

        const auto return_value = *ret;
        REQUIRE(return_value == std::to_string(new_value));
        REQUIRE(std::to_string(new_value) != value);
    }
    REQUIRE(true);
}

TEST_CASE("KVStore Tests", "[quickkv][get_bad_key]") {
    quickkv::KVStore store;
    REQUIRE(store.size() == 0);
    size_t size = 5;
    populate_database(store, size);
    REQUIRE(store.size() == size);

    auto bad_key = "not-a-good-key";
    auto value = store.get(bad_key);
    REQUIRE(!value);

    auto key = store.keys().at(0);
    value = store.get(key);
    REQUIRE(value);

    // now delete the k/v and ensure that a read-back is empty
    store.remove(key);
    value = store.get(key);
    REQUIRE(!value);
}

TEST_CASE("KVStore Tests", "[quickkv][keys]") {
    quickkv::KVStore store;
    REQUIRE(store.size() == 0);
    size_t size = 50;
    populate_database(store, size);
    REQUIRE(store.size() == size);
    auto keys = store.keys();
    REQUIRE(keys.size() == size);

    for (const auto& key : keys) {
        auto value = store.get(key);
        REQUIRE(value);
    }
}

TEST_CASE("KVStore Tests", "[quickkv][last_n]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE("KVStore Tests", "[quickkv][search]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE("KVStore Tests", "[database][read_database]") {
    // write out a raw database file key/value
    spdlog::info("raw data: {}", helpers::raw_temps_data);
    const auto path = helpers::create_temp_path("store-read-test_");

    spdlog::info("write to: {}", path.string());

    std::ofstream os(path);
    os << helpers::raw_temps_data;
    os.close();

    quickkv::KVStore store;
    REQUIRE(store.size() == 0);

    bool ok = store.read(path);
    REQUIRE(ok);
    REQUIRE(store.size() == 12);

    for (const auto& key : store.keys()) {
        auto value = store.get(key);
        if (value) {
            float v = std::stof(*value);
            REQUIRE(v < 17.0);
            REQUIRE(v > 15.0);
        } else {
            REQUIRE(false);
        }
    }

    helpers::remove_temp_path(path);
}

TEST_CASE("KVStore Tests", "[database][write_database]") {
    quickkv::KVStore store;
    REQUIRE(store.size() == 0);
    size_t size = 20;
    populate_database(store, size);
    REQUIRE(store.size() == size);

    const auto path = helpers::create_temp_path("store-write-test_");
    bool ok = store.write(path);
    REQUIRE(ok);

    quickkv::KVStore xstore;
    xstore.read(path);
    REQUIRE(xstore.size() == xstore.size());

    helpers::remove_temp_path(path);
}

TEST_CASE("KVStore tests", "[database][read,write,default databae]") {
    quickkv::KVStore store;
    REQUIRE(store.size() == 0);
    size_t size = 20;
    populate_database(store, size);
    REQUIRE(store.size() == size);

    const auto path = helpers::create_temp_path("store-write-test_");
    store.set_default_path(path);

    REQUIRE(store.get_default_path() == path);

    helpers::remove_temp_path(path);
}

TEST_CASE("KVStore Tests", "[database][append_key_value]") {
    REQUIRE(true);
}

TEST_CASE("KVStore Tests", "[database][bad_append_file]") {

    const auto filename = "bad-file/folder/temps/bad.db";
    const auto key = "12345";
    const auto sval = "22.3344";

    spdlog::info("file: {} {} {}", filename, key, sval);

    INFO("will throw if this operation failed");

    try {
        quickkv::append_key_value(filename, key, sval);
        REQUIRE(false);
    } catch (const std::exception& e) {
        spdlog::error("{}", e.what());}
        REQUIRE(true);
}

TEST_CASE("KVStore Tests", "[database][data]") {
    // create some data and store in a temp file
    quickkv::KVStore db;
    REQUIRE(db.size() == 0);

    const size_t size = 250;
    populate_database(db, size);
    // open the database and read the file
    REQUIRE(db.size() == size);

    auto keys = db.keys();

    const auto key = keys.at(25);
    REQUIRE(key.size() > 6);
    auto ret = db.get(key);
    if (ret) {
        const auto value = *ret;
        REQUIRE(value.size() > 3);
    } else {
        REQUIRE(false);
    }
}

TEST_CASE("KVStore Tests", "[database][read_data]") {
    const FilePath path = helpers::create_temp_path("tmpdb_");
    spdlog::info("file: {}", path.string());
    REQUIRE(1 == 1);

    helpers::remove_temp_path(path);
}

