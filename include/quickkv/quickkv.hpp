//
// 2025-03-14 10:28:16 dpw
//

#pragma once

#include <exception>
#include <filesystem>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// type defines
using Str = std::string;
using StrView = std::string_view;
using FilePath = std::filesystem::path;
template<typename T>
using Vec = std::vector<T>;
template<typename T>
using Func = std::function<T>;
template<typename T>
using Optional = std::optional<T>;

namespace quickkv {
    constexpr auto VERSION = "0.7.2.125";
    constexpr StrView get_version() { return VERSION; }

    using KeyType = Str;

    using FilterFunc = std::function<bool(const Str &)>;
    using SortedMap = std::map<Str, Str>;


    // a lambda to pass to KBStore::keys() (the default)
    static FilterFunc all_filter = [](const Str &) { return true; };
    constexpr auto DEFAULT_PATH = "data/store.db";


    struct KVStore {
    private:
        std::map<Str, Str> data;
        mutable std::mutex mtx; // mutable to allow locking in const methods
        FilePath default_path = DEFAULT_PATH;
        bool dirty = false;
        std::time_t last_write_seconds = 0;

    public:
        KVStore() = default;

        // Thread-safe set method; inserts or updates the key/value pair; true if
        // inserted
        bool set(const KeyType &key, const Str &value);

        // Thread-safe get method; returns data for key or an empty string if not
        // found
        // TODO replace the return with a std::expected rather than an empty string
        Optional<Str> get(const KeyType &key) const;

        // Thread-safe remove method
        bool remove(const KeyType &key) {
            std::lock_guard<std::mutex> lock(mtx);
            return data.erase(key) > 0;
        }

        // return the last n number of k/v elements
        SortedMap last(size_t count = 1) const;

        // Thread-safe keys method with optional filter function
        // returns a new Vec<Str> of keys as a copy.
        Vec<Str> keys(const FilterFunc &filter = all_filter) const;

        // search with FilterFunc returns a SortedMap of key/value pairs that match the value filter
        SortedMap search(const FilterFunc &filter = all_filter) const;

        // return the current database size
        size_t size() const;

        // read from default resource to populate database; optionally clear the store first
        bool read(bool clear = false);

        // save the current database to external source, default path or other
        bool write();

        std::time_t get_last_write_seconds() const;

        // append the key/value to the default file; throws FileException on error; clears dirty flag
        void append(const KeyType &key, const Str &value);

        // return the default path
        FilePath get_default_path() const { return default_path; }

        // set the default path
        void set_default_path(const FilePath &path) { default_path = path; };

        // return a random key/value pair.  throws if store is empty
        std::pair<KeyType, Str> random() const;

        // return true if the current kv has changes without write or append
        bool is_dirty();

        ~KVStore();

    }; // struct database

    // Base exception class
    class ServiceException : public std::exception {
    public:
        explicit ServiceException(Str message) : msg_(std::move(message)) {}
        [[nodiscard]] const char *what() const noexcept override { return msg_.c_str(); }

    private:
        Str msg_;
    };

    // Specific exceptions
    class NetworkException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

    class ParseException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

    class KBStore : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

    class FileException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

    class ValidationException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

    class KeyException : public ServiceException {
    public:
        using ServiceException::ServiceException;
    };

} // namespace quickkv
