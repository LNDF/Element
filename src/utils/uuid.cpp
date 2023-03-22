#include "uuid.h"

#include <algorithm>
#include <random>
#include <climits>
#include <memory.h>

#include <core/log.h>
#include <utils/seed.h>

static std::independent_bits_engine<std::mt19937, CHAR_BIT, unsigned char> rand_engine;

using namespace element;

uuid uuid::null_uuid("00000000-0000-0000-0000-000000000000");

uuid::uuid() {
    do this->regenerate(); while (this->is_null());
}

uuid::uuid(const uuid& uuid) {
    memcpy(this->bytes, uuid.bytes, 16);
}

uuid::uuid(const std::string& uuid) {
    this->set_from_str(uuid);
}

std::string uuid::str() const {
    
    char cs[38];
    snprintf(cs, 38, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			this->time_low, this->time_mid, this->time_hi_and_version,
			this->clk_seq_hi_res, this->clk_seq_low,
			this->node[0], this->node[1], this->node[2],
			this->node[3], this->node[4], this->node[5]);
    return std::string(cs);
}

bool uuid::operator==(const uuid& other) const {
    return memcmp(this->bytes, other.bytes, 16) == 0;
}

void uuid::regenerate() {
    std::generate(bytes, bytes + 16, std::ref(rand_engine));
    this->clk_seq_hi_res = (std::uint8_t) ((this->clk_seq_hi_res & 0x3F) | 0x80);
    this->time_hi_and_version = (std::uint16_t) ((this->time_hi_and_version & 0x0FFF) | 0x4000);
}

void uuid::set_from_str(const std::string& uuid) {
    const char* cstr = uuid.c_str();
    const char* cstrp = cstr;
    char tmp[3] = {0};
    if (uuid.size() != 36) {
        throw std::runtime_error("Incorrect UUID size");
    }
    for (int i = 0; i < 36; i++, cstrp++) {
        char c = *cstrp;
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (c == '-') {
                continue;;
            } else {
                throw std::runtime_error("Incorreect UUID format");
            }
        }
        if (!isxdigit(c)) {
            throw std::runtime_error("Incorreect UUID character");
        }
    }
    this->time_low = strtoul(cstr, NULL, 16);
    this->time_mid = strtoul(cstr + 9, NULL, 16);
    this->time_hi_and_version = strtoul(cstr + 14, NULL, 16);
    cstrp = cstr + 19;
    tmp[0] = *cstrp++;
    tmp[1] = *cstrp++;
    this->clk_seq_hi_res = strtoul(tmp, NULL, 16);
    tmp[0] = *cstrp++;
    tmp[1] = *cstrp++;
    this->clk_seq_low = strtoul(tmp, NULL, 16);
    cstrp = cstr + 24;
    for (int i = 0; i < 6; i++) {
        tmp[0] = *cstrp++;
        tmp[1] = *cstrp++;
        this->node[i] = strtoul(tmp, NULL, 16);
    }
}

bool uuid::is_null() const {
    return memcmp(this->bytes, null_uuid.bytes, 16) == 0;
}

std::size_t std::hash<uuid>::operator()(uuid const& k) const {
    std::size_t hash = 0x5f12e3bd;
    std::size_t l = 16;
    const char* p = reinterpret_cast<const char*>(k.bytes);
    for (; l; --l) {
        hash = (hash * 131) + *p++;
    }
    return hash;
}

std::ostream& operator<<(std::ostream& os, const uuid& data) {
    os << data.str();
    return os;
}

void uuid::reseed_generator() {
    uint32_t seed = get_random_seed();
    ELM_DEBUG("Setting up mt19937 RNG to generate UUIDs. Seed {0} (entropy {1})", seed, get_random_seed_entropy());
    rand_engine.seed(seed);
}