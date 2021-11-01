#pragma once

#include "pch.h"

#include "Engine/Core/UUID.hpp"

#include <limits>
#include <unordered_set>

namespace a2de {
    TEST(UUID, FirstTenMillionUUIDsAreUnique) {
        std::unordered_set<a2de::UUID> set;

        bool success = true;
        for(auto i = uint64_t{0}; i != uint64_t{10000000ull}; ++i) {
            const auto [_, successful] = set.insert(a2de::UUID());
            success &= successful;
            EXPECT_TRUE(success);
            if(!success) {
                std::cout << "UUID ID generation failed after " << set.size() << " IDs.\n";
                break;
            }
        }
    }

}