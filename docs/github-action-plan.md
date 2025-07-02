# Github Action Plan

  1. Domain-keys: Now external dependency (not in domain-keys/ subdirectory)
  2. Dependency Strategy: Need to either:
    - Install domain-keys from system packages
    - Build domain-keys from separate repository
    - Use prebuilt domain-keys artifacts

  Build Steps (Revised)

  1. Setup Ubuntu 24.04 runner with C++23 toolchain
  2. Install system dependencies (cmake, ninja, gcc-14.2, nlohmann-json, catch2)
  3. Handle domain-keys dependency:
    - Option A: Install from package manager if available
    - Option B: Clone and build domain-keys from separate repo
    - Option C: Download prebuilt domain-keys library
  4. Configure CMake with Release build type and -DBUILD_TESTING=OFF for Catch2
  5. Build quickkv library (will link against domain-keys)
  6. Run unit tests
  7. Package artifacts

  Questions for Implementation

  - Where is the domain-keys dependency now located? (separate repo, system package, etc.)
  - Should the workflow build domain-keys from source or use prebuilt?
  - What's the preferred method for obtaining the domain-keys dependency?

  The CMakeLists.txt still references domainkeys target, so the workflow needs to ensure this dependency is available before building quickkv.
