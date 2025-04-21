//
// 2025-02-04 17:01:53 dpw
//

#include <algorithm>
#include <fstream>
#include <quickkv/quickkv.hpp>
#include <ranges>
#include <spdlog/spdlog.h>
#include <sstream>

/*
 * create a k/v compatible with future redis integration
 */
namespace quickkv {
    // append the key/value to the file; throws on error; returns the number of
    // bytes written
    void append_key_value(const FilePath &path, const KeyType &key, const Str &value) {
        std::ofstream file(path, std::ios::app);

        if (!file.is_open()) {
            const auto msg = "Error in database::append_key_value(); can't open file: " + path.string();
            spdlog::error(msg);
            throw FileException(msg);
        }

        file << key << "=" << value << '\n';
        file.close();
    }

    bool read_current_data(KVStore &store) {
        // TODO read from configuration, all database files
        Str location = "cottage";
        const FilePath path = "data/temperature/current." + location + ".store";

        spdlog::info("read current data from {}", path.string());
        store.read(path, false);
        spdlog::info("store size: {}", store.size());

        return true;
    }

    bool KVStore::set(const KeyType &key, const Str &value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (data.contains(key)) {
            data[key] = value;
            return false;
        } else {
            data.emplace_hint(data.end(), key, value);
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

        std::ranges::copy_if(data, std::inserter(map, map.end()), [&](const auto &pair) { return filter(pair.first); });

        return map;
    }

    size_t KVStore::size() const { return data.size(); }

    // Thread-safe read from file
    bool KVStore::read(const FilePath &path, bool clear) {
        std::lock_guard<std::mutex> lock(mtx);
        std::ifstream infile(path);
        if (!infile.is_open()) {
            return false;
        }

        if (clear) {
            spdlog::info("clearing the database prior to read");
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
        return true;
    }

    // Thread-safe dump/save to file
    bool KVStore::write(const FilePath &path) const {
        std::lock_guard<std::mutex> lock(mtx);
        std::ofstream outfile(path);
        if (!outfile.is_open()) {
            return false;
        }
        for (const auto &[key, value]: data) {
            outfile << key << "=" << value << "\n";
        }
        return true;
    }

} // namespace quickkv
