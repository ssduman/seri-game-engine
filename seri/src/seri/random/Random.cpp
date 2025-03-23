#include "Seripch.h"
#include "seri/random/Random.h"

namespace seri {
    static std::random_device _rd;
    static std::mt19937_64 _gen = std::mt19937_64(_rd());
    static std::uniform_int_distribution<unsigned long long> _distrib;

    Random::Random() : _random(_distrib(_gen)) {}

    unsigned long long Random::get() {
        return _random;
    }
}
