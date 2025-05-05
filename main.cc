#include <iostream>
#include <fstream>
#include "Club.h"
#include "EventFactory.h"
#include "EventReader.h"

std::tuple<bool, uint64_t, uint64_t, uint64_t, uint64_t> parseInitParams(std::ifstream& in) {
    std::string s;
    std::getline(in, s);
    EventReader er(in, 2);
    auto [countOfTabels, ok] = er.toInt(s);
    if (!ok || !countOfTabels) {
        std::cout << 1 << '\n';
        return {false,{},{},{},{}};
    }

    std::getline(in, s);
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

    std::getline(in, s);
    auto [price, flag] = er.toInt(s);
    if (!flag || !price) {
        std::cout << 3 << '\n';
        return {false,{},{},{},{}};
    }
    return {true, countOfTabels, sTime, eTime, price};
}


int main(int argc, char** argv) {
    std::ifstream in;
    if (argc < 2) {
        std::cout << "File was not specified: " << '\n';
        return EXIT_SUCCESS;
    }
    in.open(argv[1]);
    if (!in.is_open()) {
        std::cout << "Can not open file: " << argv[1] << '\n';
        return EXIT_SUCCESS;
    }
    auto [flag, countOfTabels, sTime, eTime, price] = parseInitParams(in);
    if (!flag) return EXIT_SUCCESS;


    EventReader er(in, 4, countOfTabels);
    EventFactory eventFactory(er);
    Club club(countOfTabels, sTime, eTime, price);
    try {
        for (;;) {
            auto event = eventFactory.create();
            club.addEvent(std::move(event));
        }
    } catch (const ParseError& pe) {
        std::cout << pe.what() << '\n';
        return EXIT_SUCCESS;
    } catch (const EndOfFile& eof) {

    }
    std::cout << timeToString(sTime) << '\n';
    club.processAll();
    club.manager_.kickOutAll();
    std::cout << timeToString(eTime) << '\n';

    club.manager_.showStats();
}
