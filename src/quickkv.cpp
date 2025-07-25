//
// 2025-02-04 17:01:53 dpw
//

#include <algorithm>
#include <domainkeys/keys.hpp>
#include <fstream>
#include <optional>
#include <quickkv/quickkv.hpp>
#include <random>
#include <ranges>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/*
 * create a k/v compatible with future redis integration
 */
namespace quickkv {
    // Seed the random number generator
    std::random_device rdev;
    std::mt19937 generator(rdev());

    bool KVStore::set(const KeyType &key, const Str &value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (data.contains(key)) {
            data[key] = value;
            dirty = true;
            return false;
        } else {
            data.emplace_hint(data.end(), key, value);
            dirty = true;
            return true;
        }
    }

    Optional<Str> KVStore::get(const KeyType &key) const {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        } else {
            return std::nullopt;
        }
    }

    SortedMap KVStore::last(const size_t count) const {
        SortedMap result;
        std::lock_guard<std::mutex> lock(mtx);

        if (count <= 0 || data.empty())
            return result; // Handle edge cases

        // Create a view that skips the first (size - count) elements
        auto last_view = data | std::views::drop(data.size() > count ? data.size() - count : 0);

        // Copy selected elements into result (still sorted)
        std::ranges::copy(last_view, std::inserter(result, result.end()));

        return result; // Already in sorted order
    }

    // Thread-safe keys method with optional filter; returns sorted vector
    // returns a new Vec<Str> of keys as a copy.
    Vec<Str> KVStore::keys(const FilterFunc &filter) const {
        Vec<Str> key_list;

        std::lock_guard<std::mutex> lock(mtx);

        // extract keys into views::keys then filter
        std::ranges::copy_if(data | std::views::keys, std::back_inserter(key_list), filter);

        return key_list;
    }

    SortedMap KVStore::search(const FilterFunc &filter) const {
        SortedMap map;

        std::lock_guard<std::mutex> lock(mtx);

        std::ranges::copy_if(data, std::inserter(map, map.end()),
                             [&](const auto &pair) { return filter(pair.second); });

        return map;
    }

    // return a random key/value pair
    std::pair<KeyType, Str> KVStore::random() const {
        std::lock_guard<std::mutex> lock(mtx);
        if (data.empty()) {
            throw ServiceException("Database is empty");
        }

        const auto pairs = data | std::views::all;
        const Vec<std::pair<KeyType, Str>> vec(pairs.begin(), pairs.end());

        std::uniform_int_distribution<> distribution(0, vec.size() - 1);

        // Get a random index
        const int idx = distribution(generator);

        return vec[idx];
    }

    // return the current store size
    size_t KVStore::size() const { return data.size(); }

    // Thread-safe read from file
    bool KVStore::read(bool clear) {
        bool currently_dirty = dirty;
        std::lock_guard<std::mutex> lock(mtx);
        std::ifstream infile(default_path);
        if (!infile.is_open()) {
            return false;
        }

        if (clear) {
            spdlog::debug("clearing the database prior to read");
            data.clear();
        }

        Str line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            KeyType key;
            Str value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                data[key] = value;
            }
        }

        // restore the dirty flag
        dirty = currently_dirty;

        return true;
    }

    // Thread-safe dump/save to file
    bool KVStore::write() {
        std::lock_guard<std::mutex> lock(mtx);
        std::ofstream outfile(default_path);
        if (!outfile.is_open()) {
            return false;
        }

        for (const auto &[key, value]: data) {
            outfile << key << "=" << value << "\n";
        }

        dirty = false;
        last_write_seconds = domainkeys::keys::now_microseconds() / 1'000'000;

        return true;
    }

    std::time_t KVStore::get_last_write_seconds() const { return last_write_seconds; }

    // append the key/value to the file; throws on error; returns the number of
    void KVStore::append(const KeyType &key, const Str &value) {
        std::lock_guard<std::mutex> lock(mtx);
        std::ofstream file(default_path, std::ios::app);

        if (!file.is_open()) {
            const auto msg = "Error in database::append_key_value(); can't open file: " + default_path.string();
            spdlog::critical(msg);
            throw FileException(msg);
        }

        file << key << "=" << value << "\n";
        spdlog::debug("wrote key/value: {}={}", key, value);

        dirty = false;

        file.close();
    }

    // dirty flag
    bool KVStore::is_dirty() { return dirty; }

    KVStore::~KVStore() {
        if (dirty) {
            auto path = get_default_path();
            spdlog::info("KVStore destructor, dirty: {}, writing to file: {}", dirty, path.string());
            // write(path);
        }
    }

} // namespace quickkv
