#include "../surreals.h"
#include <iostream>
#include <string>

using namespace surreals;

void Mult() {
    std::cout << "Please input a pair if integers to convert: " << std::endl;
    int a, b;
    char inp;

    if ((std::cin >> a) && (std::cin >> b)) {
        Surreal aS = Surreal(a), bS = Surreal(b);

        std::cout << "Multiplying... (this might take a while for numbers with depth > 10)" << std::endl;

        Surreal res = aS * bS;

        std::cout << std::endl << "result: " << res << std::endl;

        std::cout << "The addition table has "<< Surreal::AddLookup.size() << " entries. Print them out ? (y/n)" << std::endl;
        std::cin >> inp;
        if (inp == 'y') {
            for (auto elem : Surreal::AddLookup) {
                std::cout << elem.first.first << " + " << elem.first.second << " = " << elem.second << std::endl;
            }
        }

        std::cout << "The multiplication table has "<< Surreal::MultLookup.size() << " entries. Print them out ? (y/n)" << std::endl;
        std::cin >> inp;
        if (inp == 'y') {
            for (auto elem : Surreal::MultLookup) {
                std::cout << elem.first.first << " * " << elem.first.second << " = " << elem.second << std::endl;
            }
        }

        std::cout << "Continue ? (y/n)" << std::endl;
        std::cin >> inp;
        if (inp == 'y') {
            Mult();
        }

    }
}

int main() {

    std::cout << "This is a short demo showcasing 'finite' Surreal numbers." <<
              std::endl <<
              std::endl << "Enter two integers to multiply. The demo will print out the result" <<
              std::endl << "along with the generated addition and multiplication tables." <<
              std::endl << std::endl;

    Mult();
}