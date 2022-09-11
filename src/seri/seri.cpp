#include "RunnerSeri.h"

int main(int argc, char* argv[]) {
    try {
        RunnerSeri{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
