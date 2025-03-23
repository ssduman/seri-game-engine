#pragma once

namespace seri {

    struct Random {
        Random();

        unsigned long long get();

    private:
        unsigned long long _random;

    };
}