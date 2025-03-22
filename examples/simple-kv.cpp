


#include <print>
#include <iostream>
#include "quickkv/quickkv.hpp" // Adjust this include based on your actual header file

int main() {
    // Create an instance of your key-value store
    quickkv::Database db;
    std::println("db size: {}", db.size());

    // Insert some key-value pairs
    db.set("key1", "value1");
    db.set("key2", "value2");

    Optional<Str> value = db.get("key1");
    if (value.has_value()) {
        std::println("key1: {}", value.value());
    } else {
        std::println("ERROR key1");
    }

    value = db.get("key2");
    if (value.has_value()) {
        std::println("key2: {}", value.value());
    } else {
        std::println("ERROR key1");
    }

    std::println("db size: {}", db.size());

    return 0;
}

