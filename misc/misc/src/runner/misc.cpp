#include "RunnerMisc.h"

int main(int argc, char* argv[]) {
    try {
        RunnerMisc{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
