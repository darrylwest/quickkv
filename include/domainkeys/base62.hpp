//
// dpw
//

#pragma once

#include <algorithm>
#include <string>


namespace domainkeys::base62 {
    static constexpr char base62_chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    auto encodeBase62(long value) {
        if (value == 0) {
            return std::string(1, base62_chars[0]); // Handle the case for 0
        }

        std::string result;
        while (value > 0) {
            std::uint64_t remainder = value % 62;
            result.push_back(base62_chars[remainder]);
            value /= 62;
        }

        // The result is in reverse order, so we need to reverse it
        std::reverse(result.begin(), result.end());
        return result;
    }
} // namespace domainkeys::base62
