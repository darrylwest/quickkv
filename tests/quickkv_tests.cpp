//
// Created by Darryl West on 2/27/25.
//
#include <quickkv/quickkv.hpp>
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <filesystem>
#include <print>

#include "test_helpers.hpp"

void populate_database(quickkv::Database& db, const size_t size = 100) {

    for (size_t i = 0; i < size; ++i) {
        auto key = std::to_string(helpers::random_int());
        auto value = helpers::random_float(20.0, 30.0);

        db.set(key, std::to_string(value));
    }
}

TEST_CASE("Quick KV Tests", "[quickkv][version]") {

    auto vers = quickkv::get_version();
    std::println("Version: {}", vers);

    REQUIRE(vers.starts_with("0.3."));
}

TEST_CASE("Quick KV Tests", "[quickkv][set_get]") {
    quickkv::Database db;
    REQUIRE(db.size() == 0);
    size_t size = 10;
    populate_database(db, size);
    REQUIRE(db.size() == size);

    auto keys = db.keys();

    for (const auto& key : keys) {
        INFO("find for key: " << key);
        auto ret = db.get(key);
        if (ret) {
            REQUIRE(true);
        } else {
            REQUIRE(false);
            continue;
        }

        const auto value = *ret;

        auto new_value = helpers::random_float(30.0, 40.0);
        auto resp = db.set(key, std::to_string(new_value));
        INFO("replacing/updating a value should always return false");
        REQUIRE(resp == false);
        ret = db.get(key);
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

TEST_CASE("Database Tests", "[quickkv][get_bad_key]") {
    quickkv::Database db;
    REQUIRE(db.size() == 0);
    size_t size = 5;
    populate_database(db, size);
    REQUIRE(db.size() == size);

    auto bad_key = "not-a-good-key";
    auto value = db.get(bad_key);
    REQUIRE(!value);

    auto key = db.keys().at(0);
    value = db.get(key);
    REQUIRE(value);

    // now delete the k/v and ensure that a read-back is empty
    db.remove(key);
    value = db.get(key);
    REQUIRE(!value);
}

TEST_CASE("Database Tests", "[quickkv][keys]") {
    quickkv::Database db;
    REQUIRE(db.size() == 0);
    size_t size = 50;
    populate_database(db, size);
    REQUIRE(db.size() == size);
    auto keys = db.keys();
    REQUIRE(keys.size() == size);

    for (const auto& key : keys) {
        auto value = db.get(key);
        REQUIRE(value);
    }
}

TEST_CASE("Database Tests", "[quickkv][last_n]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE("Database Tests", "[quickkv][search]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE("Database Tests", "[database][read_database]") {
    // write out a raw database file key/value
    spdlog::info("raw data: {}", helpers::raw_temps_data);
    const auto path = helpers::create_temp_path("db-read-test_");

    spdlog::info("write to: {}", path.string());

    std::ofstream os(path);
    os << helpers::raw_temps_data;
    os.close();

    quickkv::Database db;
    REQUIRE(db.size() == 0);

    bool ok = db.read(path);
    REQUIRE(ok);
    REQUIRE(db.size() == 12);

    for (const auto& key : db.keys()) {
        auto value = db.get(key);
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

TEST_CASE("Database Tests", "[database][write_database]") {
    quickkv::Database db;
    REQUIRE(db.size() == 0);
    size_t size = 20;
    populate_database(db, size);
    REQUIRE(db.size() == size);

    const auto path = helpers::create_temp_path("db-write-test_");
    bool ok = db.save(path);
    REQUIRE(ok);

    quickkv::Database rdb;
    rdb.read(path);
    REQUIRE(rdb.size() == db.size());

    helpers::remove_temp_path(path);
}

TEST_CASE("Database Tests", "[database][append_key_value]") {
    REQUIRE(true);
}

TEST_CASE("Database Tests", "[database][bad_append_file]") {

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

TEST_CASE("Database Tests", "[database][data]") {
    // create some data and store in a temp file
    quickkv::Database db;
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

TEST_CASE("Database Tests", "[database][read_data]") {
    const FilePath path = helpers::create_temp_path("tmpdb_");
    spdlog::info("file: {}", path.string());
    REQUIRE(1 == 1);

    helpers::remove_temp_path(path);
}

