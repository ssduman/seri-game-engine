#include "RunnerTetris.h"

int main(int argc, char* argv[]) {
    try {
        RunnerTetris{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
