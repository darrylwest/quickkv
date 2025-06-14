//
// 2025-03-26 10:28:16 dpw
//

#include <quickkv/quickkv.hpp>
#include <domainkeys/keys.hpp>
#include <print>
#include <nlohmann/json.hpp>
#include <optional>
#include <vendor/perftimer.hpp>

using json = nlohmann::json;

int main() {
    perftimer::PerfTimer timer("Read Data");
    std::println("Read from disk, verify size, and print first 10 keys");
    quickkv::KVStore store;
    std::println("new db size: {}", store.size());

    store.set_default_path("data/data-file.db");
    std::println("set the default file path: {}", store.get_default_path().string());

    timer.start();
    if (store.read() && store.size() == 1000) {
        timer.stop();
        timer.show_duration();
        std::println("new size: {}", store.size());
    } else {
        std::println("Failed to read all data from disk, db size: {}", store.size());
        return 1;
    }

    auto keys = store.keys();
    int count = 0;
    for (const auto& key : keys) {
        const Optional<Str> value = store.get(key);
        if (value.has_value()) {
            if (count++ < 10) {
                std::println("{} = {}", key, value.value());
            }
        } else {
            std::println("ERROR key: {}", key);
        }
    }

    store.set_default_path("data/data-file.backup");
    const auto key = "80uEPvuHCyCz";
    store.set(key, "my new value");

    std::println("store is dirty {} so write to disk, last write: {}", store.is_dirty(), store.get_last_write_seconds());

    timer.start();
    store.write();
    timer.stop();
    timer.show_duration();

    std::println("store is dirty {} so write to disk, last write: {}", store.is_dirty(), store.get_last_write_seconds());

    return 0;
}
