#include "../include/StompProtocol.h"
#include <stdlib.h>
#include <string>

int main(int argc, char* argv[]) {
    StompProtocol protocol;

    while (1) {
        std::string input;
        std::cout << "Enter command -> " << std::flush;
        std::getline(std::cin, input);
        try {
            protocol.handleUserInput(input);
        } catch (const std::exception& e) {
            std::cerr << "Unknown error occurred on input" << std::endl;
        }
    }

    return 0;
}
