#include "RunnerFractal.h"

int main(int argc, char* argv[]) {
    try {
        RunnerFractal{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
