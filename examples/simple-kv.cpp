


#include <quickkv/quickkv.hpp>
#include <print>
#include <fmt/core.h>

void populate_database(quickkv::KVStore& store, const size_t size = 100) {

    for (size_t i = 0; i < size; ++i) {
        auto key = std::to_string(i + 1000);
        auto value = fmt::format("{} thing", key);

        store.set(key, value);
    }
}

int main() {
    std::println("Running a simple example that creates a 100 element kv store and shows the first 10...");
    // Create an instance of your key-value store
    //
    quickkv::KVStore store;
    populate_database(store);

    std::println("after fill, db size: {}", store.size());

    const auto keys = store.keys();

    for (int i = 0; i < 10; ++i) {
        const auto& key = keys[i];
        const Optional<Str> value = store.get(key);
        if (value.has_value()) {
            std::println("{}) key {}: value: {}", i, key, value.value());
        } else {
            std::println("ERROR key1");
        }
    }

    return 0;
}

