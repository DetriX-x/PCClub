#include <iostream>
#include "Club.h"
#include "EventFactory.h"
#include "EventReader.h"

std::tuple<bool, uint64_t, uint64_t, uint64_t, uint64_t> parseInitParams() {
    std::string s;
    std::getline(std::cin, s);
    EventReader er(2);
    auto [countOfTabels, ok] = er.toInt(s);
    if (!ok || !countOfTabels) {
        std::cout << 1 << '\n';
        return {false,{},{},{},{}};
    }

    std::getline(std::cin, s);
    auto splitted = er.split(s, ' ');
    if (splitted.size() != 2) {
        std::cout << 2 << '\n';
        return {false,{},{},{},{}};
    }
    std::string startTime = splitted[0];
    std::string endTime = splitted[1];
    uint64_t sTime {};
    uint64_t eTime {};
    try {
        sTime = er.getTimeInSecs(startTime);
        eTime = er.getTimeInSecs(endTime);
    } catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        return {false,{},{},{},{}};
    }

    std::getline(std::cin, s);
    auto [price, flag] = er.toInt(s);
    if (!flag || !price) {
        std::cout << 3 << '\n';
        return {false,{},{},{},{}};
    }
    return {true, countOfTabels, sTime, eTime, price};
}


int main() {
    auto [flag, countOfTabels, sTime, eTime, price] = parseInitParams();
    if (!flag) return EXIT_SUCCESS;


    EventReader er(4, countOfTabels);
    EventFactory eventFactory(er);
    Club club(countOfTabels, sTime, eTime, price);
    try {
        for (;;) {
            auto event = eventFactory.create();
            club.addEvent(event);
        }
    } catch (const ParseError& pe) {
        std::cout << pe.what() << '\n';
        return EXIT_SUCCESS;
    } catch (const EndOfFile& eof) {

    }
    std::cout << timeToString(sTime) << '\n';
    club.processAll();
    club.kickOutAll();
    std::cout << timeToString(eTime) << '\n';

    club.showStats();
}
