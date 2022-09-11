#include "RunnerEditor.h"

int main(int argc, char* argv[]) {
    try {
        RunnerEditor{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
