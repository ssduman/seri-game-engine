#include "Runner.h"

int main(int argc, char* argv[]) {
    try {
        Runner{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occurred: " << ex.what());
    }

    return 0;
}
