#include "seed.h"

static std::random_device device;

namespace element {

    uint32_t get_random_seed() {
        return device();
    }

    double get_random_seed_entropy() {
        return device.entropy();
    }

} // namespace element