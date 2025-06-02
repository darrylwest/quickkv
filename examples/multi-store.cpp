//
// Created by Darryl West on 6/2/25.
//

#include <print>
#include <array>
#include <map>
#include <string>
#include <memory>

#include <quickkv/quickkv.hpp>
#include <vendor/perftimer.hpp>

std::array<std::string, 4> store_names = {
    "contact-list",
    "users",
    "blog-posts",
    "data-file",
};

std::map<std::string, std::unique_ptr<quickkv::KVStore>> create_stores() {
    std::map<std::string, std::unique_ptr<quickkv::KVStore>> stores;

    for (const auto& name : store_names) {
        auto path = "data/" + name + ".db";

        auto ptr = std::make_unique<quickkv::KVStore>();
        ptr->set_default_path(path);
        ptr->read(path);

        stores[name] = std::move(ptr);
    }

    return stores;
}

int main() {
    std::puts("Creating multiple stores");

    perftimer::PerfTimer timer("Multi Store");
    std::println("Running a multi store example...");

    timer.start();

    const auto stores = create_stores();

    timer.stop();
    timer.show_duration();

    for (const auto& [path, store] : stores) {
        std::println("store: {} size: {}", path, store->size());
    }

    auto it = stores.find("contact-list");
    if (it != stores.end()) {
        auto store = it->second.get();

        std::println("contacts: {}", store->size());
        auto last5 = store->last(5);
        for (const auto& [key, value] : last5) {
            std::println("key: {} value: {}", key, value);
        }
    }

    return 0;
}