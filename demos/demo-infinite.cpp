#include "../surreals.h"
#include <iostream>
#include <string>

using namespace surreals;


int main() {

    std::function<SurrealInf(int)> const naturals = [](int inp) {
        return SurrealInf(Surreal(inp));
    };

    std::function<SurrealInf(int)> const neg_naturals = [](int inp) {
        return SurrealInf(-inp);
    };

    std::function<SurrealInf(int)> const fracs = [](int inp) {
        return SurrealInf((float) pow(2, -inp));
    };

    SurrealInf zeroInf = SurrealInf(
            [](int) {
                return SurrealInf();
            },
            [](int) {
                return SurrealInf();
            },
            std::make_pair(0,0));

    std::cout << "printing Zero.." << std::endl;
    std::cout << zeroInf << std::endl;

    SurrealInf twoInf = SurrealInf(2);
    std::cout << "printing Two.." << std::endl;
    std::cout << twoInf << std::endl;

    SurrealInf omega = SurrealInf(naturals, nullptr, std::make_pair(-1,0));
    std::cout << "printing Omega.." << std::endl;
    std::cout << omega << std::endl;

    SurrealInf neg_omega = SurrealInf(nullptr, neg_naturals, std::make_pair(0,-1));
    std::cout << "printing Negative Omega.." << std::endl;
    std::cout << neg_omega << std::endl;

    SurrealInf epsilon = SurrealInf(nullptr, fracs, std::make_pair(0,-1));
    std::cout << "printing Epsilon.." << std::endl;
    std::cout << epsilon.Print(5, 1) << std::endl;
}