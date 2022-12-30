#include "RunnerMics.h"

int main(int argc, char* argv[]) {
    try {
        RunnerMics{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
