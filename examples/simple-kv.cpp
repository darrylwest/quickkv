


#include <quickkv/quickkv.hpp>
#include <domainkeys/keys.hpp>
#include <print>

void populate_database(quickkv::KVStore& store, const size_t size = 1000) {

    for (size_t i = 0; i < size; ++i) {
        auto key = domainkeys::keys::create_timestamp_key().to_string();
        auto value = domainkeys::keys::create_random_key(24);

        store.set(key, value);
    }
}

int main() {
    std::println("Running a simple example that creates a 1000 element kv store and shows the first 10...");
    // Create an instance of your key-value store

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

