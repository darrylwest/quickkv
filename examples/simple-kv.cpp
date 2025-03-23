


#include <quickkv/quickkv.hpp>
#include <print>

void populate_database(quickkv::KVStore& store, const size_t size = 100) {

    for (size_t i = 0; i < size; ++i) {
        auto key = std::to_string(i + 1000);
        auto value = key + ":value";

        store.set(key, value);
    }
}

int main() {
    // Create an instance of your key-value store
    quickkv::KVStore store;
    populate_database(store);

    std::println("after fill, db size: {}", store.size());


    const auto keys = store.keys();

    const auto key = keys[20];
    Optional<Str> value = store.get(key);
    if (value.has_value()) {
        std::println("key {}: {}", key, value.value());
    } else {
        std::println("ERROR key1");
    }

    return 0;
}

