#include "../surreals.h"
#include <iostream>
#include <string>

using namespace surreals;

void CalcDay(std::set<Surreal> &sKnown) {
    std::set<Surreal> tempKnown = sKnown;
    /// try singles
    auto it_a = sKnown.begin();
    while (it_a != sKnown.end()) {
        tempKnown.emplace(Surreal(std::set<Surreal>({*it_a}), std::set<Surreal>(), true));
        tempKnown.emplace(Surreal(std::set<Surreal>(), std::set<Surreal>({*it_a}), true));
        it_a++;
    }

    /// try pairs
    it_a = sKnown.begin();
    auto it_b = it_a;
    it_b++;

    while (it_a != sKnown.end()) {
        while (it_b != sKnown.end()) {
            tempKnown.emplace(Surreal(*it_a, *it_b));
            it_b++;
        }
        it_a++;
        it_b = it_a;
        it_b++;
    }
    sKnown = tempKnown;
}

int main() {

    int input;

    std::cout << "This is a short demo showcasing 'finite' Surreal numbers." <<
              std::endl <<
              std::endl << "We start at Day 0 with the number { | }, then on each consequent" <<
              std::endl << "day we construct new numbers using the ones we already have." <<
              std::endl <<
              std::endl << "For each known number A, we try { A | } and { | A }," <<
              std::endl << " and for each known pair A < B we try { A | B }." <<
              std::endl << std::endl;
    std::cout << "Input target day (the calculation will pause after target day): ";

    if (!(std::cin >> input)) {
        std::cout << "couldn't parse day number." << std::endl;
        return 0;
    }

    int day_target = input, day_current = 0;
    std::set<Surreal> sKnown;
    sKnown.emplace(Surreal());

    while (day_current <= day_target) {
        std::cout << "Calculating numbers for day " << day_current << "..." << std::endl << std::endl;

        CalcDay(sKnown);
        day_current++;
        if (day_current > day_target) {
            char inp;
            std::cout << "Target day achieved. There are now " << sKnown.size() << " known numbers." <<
                      std::endl << "Print them out? (y/n)" << std::endl;

            std::cin >> inp;
            if (inp == 'y') {
                std::cout << "Known numbers: " << sKnown.size() << std::endl;
                for (Surreal const &num : sKnown) {
                    std::cout << num.Float() << "\t\t= " << num << std::endl;
                }
                std::cout << std::endl;
            }

            std::cout << "Continue? (y/n)" << std::endl;
            std::cin >> inp;
            if (inp == 'y') {
                day_target++;
            }
        }
    }
}