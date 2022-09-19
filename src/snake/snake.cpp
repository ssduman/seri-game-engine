#include "RunnerSnake.h"

int main(int argc, char* argv[]) {
    try {
        RunnerSnake{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
