# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

Initialize build system:
```bash
./mk init
```

Build the project:
```bash
./mk build
```

Run unit tests:
```bash
./mk unit
# or
./mk test
```

Run examples:
```bash
./mk examples
```

Format code:
```bash
./mk format
```

Watch for changes and auto-rebuild:
```bash
./mk watch
```

Clean build artifacts:
```bash
./mk clean      # Remove binaries but keep build folder
./mk clobber    # Remove entire build folder
```

## Testing

- Tests use Catch2 v3 framework
- Test files: `tests/quickkv_tests.cpp`, `tests/main.cpp`
- Domain keys tests: `domain-keys/tests/keys_tests.cpp`
- Tests are built automatically when `ENABLE_TESTS=ON` (default)

## Architecture Overview

**QuickKV** is a thread-safe in-memory key-value store with persistent file-based backing storage.

### Core Components

- **KVStore**: Main class in `quickkv` namespace providing thread-safe operations
- **Domain Keys**: Separate `domainkeys` library for generating 12-character base62 keys with timestamp+random components
- **File Persistence**: JSON-based storage to filesystem with dirty flag tracking

### Key Features

- Thread-safe operations using std::mutex
- In-memory storage with optional file persistence
- Key generation with collision probability < 0.00128%
- Filter-based search and key retrieval
- Append-only file operations for performance

### Project Structure

- `include/quickkv/quickkv.hpp`: Main header with KVStore class
- `src/quickkv.cpp`: Implementation of KVStore methods
- `domain-keys/`: Separate CMake project for key generation
- `examples/`: Usage examples (simple-kv, disk-read-write, multi-store)
- `data/`: Default location for database files

### Dependencies

- C++23 standard required
- spdlog for logging
- nlohmann_json for JSON serialization
- Catch2 v3 for testing
- CPM.cmake for dependency management

## Development Notes

- Uses modern C++ features (C++23)
- Type aliases: `Str`, `StrView`, `FilePath`, `Vec<T>`, `Optional<T>`
- Thread-safe design with mutex locking
- Exception hierarchy for error handling
- Default database path: `data/store.db`