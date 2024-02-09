#include "Vanguard.h"
#include <Windows.h>
#include "xorstr.h"

uint64_t fnv1a64(const void* data, size_t size) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < size; i++) {
        hash ^= ((uint8_t*)data)[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

void bypass_crc(void* league_section_address, size_t league_section_size) {
    LeagueHash* league_hash_array = (LeagueHash*)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("LeagueHashArray").crypt_get());

    for (size_t i = 0; i < league_section_size; i += 0x1000) {
        uint64_t hash = fnv1a64((void*)((uint8_t*)league_section_address + i), 0x1000);
        hash ^= league_hash_array[i / 0x1000].xor_key;

        if (hash != league_hash_array[i / 0x1000].xored_hash) {
            hash = fnv1a64((void*)((uint8_t*)league_section_address + i), 0x1000);
            hash ^= league_hash_array[i / 0x1000].xor_key;
            league_hash_array[i / 0x1000].xored_hash = hash;
        }
    }
}