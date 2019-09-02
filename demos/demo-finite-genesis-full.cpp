#include "../surreals.h"
#include <iostream>
#include <string>

using namespace surreals;

void CalcDay(std::set<Surreal> &sKnown, std::set<Surreal> &sNew) {
    std::set<Surreal> tempKnown = sKnown;
    sNew.clear();

    /// try singles
    auto it_a = sKnown.begin();
    while (it_a != sKnown.end()) {
        tempKnown.emplace(Surreal(std::set<Surreal>({*it_a}), std::set<Surreal>(), true));
        tempKnown.emplace(Surreal(std::set<Surreal>(), std::set<Surreal>({*it_a}), true));
        tempKnown.emplace(-(*it_a));
        it_a++;
    }

    /// try pairs
    it_a = sKnown.begin();
    auto it_b = it_a;
    it_b++;

    while (it_a != sKnown.end()) {
        while (it_b != sKnown.end()) {
            tempKnown.emplace(Surreal(*it_a, *it_b));
            tempKnown.emplace(*it_a * (*it_b));
            tempKnown.emplace(*it_a + (*it_b));
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
              std::endl << "For each known number A, we try { A | }, { | A }, and -A." <<
              std::endl << "For each known pair A < B we try { A | B }, A + B and A * B." <<
              std::endl << std::endl;

    int day_target = 1, day_current = 0;
    std::set<Surreal> sKnown, sNew;
    sKnown.emplace(Surreal());

    while (day_current <= day_target) {
        std::cout << "Calculating numbers for day " << day_current << "..." << std::endl << std::endl;

        CalcDay(sKnown, sNew);
        day_current++;
        if (day_current > day_target) {
            char inp;
            std::cout << "There are now " << sKnown.size() << " known numbers." <<
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