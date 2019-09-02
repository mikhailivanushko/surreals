#include "../surreals.h"
#include <iostream>
#include <string>

using namespace surreals;

void ConvertFloat() {
    std::cout << "Please input a float to convert: ";
    double input;
    char ans;
    if (std::cin >> input) {
        Surreal res = Surreal((float) input);

        std::cout << res.Float() << " = " << res.Print(0) << " with depth = " << res.Depth() << std::endl;

        std::cout << std::endl << "Print out verbose version? (y/n)" << std::endl;
        std::cin >> ans;
        if (ans == 'y') {
            std::cout << std::endl << res.PrintVerbose() << std::endl;
        }

        std::cout << std::endl << "Convert another? (y/n)" << std::endl;
        std::cin >> ans;
        if (ans == 'y') {
            ConvertFloat();
        }
    } else {
        std::cout << std::endl << "could not parse the number. Please try again" << std::endl;
        ConvertFloat();
    }
}

int main() {

    std::cout << "This demo converts a 32 bit float into a surreal number." <<
              std::endl << std::endl;

    ConvertFloat();

}