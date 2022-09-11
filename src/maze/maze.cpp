#include "RunnerMaze.h"

int main(int argc, char* argv[]) {
    try {
        RunnerMaze{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
