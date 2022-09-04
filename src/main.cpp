#include "Logger.h"
#include "SeriRunner.h"
#include "MazeRunner.h"

int main(int argc, char** argv) {
    try {
        SeriRunner{}();
        //MazeRunner{}();
    } catch (const std::exception& ex) {
        LOGGER(error, "exception occured: " << std::string(ex.what()));
    }

    return 0;
}
