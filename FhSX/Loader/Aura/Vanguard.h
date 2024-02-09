#pragma once

#include <cstddef>
#include <cstdint>

uint64_t fnv1a64(const void* data, size_t size);

void bypass_crc(void* league_section_address, size_t league_section_size);


struct LeagueHash {
    uint64_t xored_hash;
    uint64_t xor_key;
};