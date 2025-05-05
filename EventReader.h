#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <sstream>
#include <iostream>
#include <optional>
#include <vector>
#include <tuple>
#include <exception>

using std::cin, std::cout, std::string, std::stringstream,
      std::exception, std::tuple, std::unique_ptr, std::optional, std::pair,
      std::to_string, std::vector;

class EndOfFile : public exception {
    string msg;
public:
    explicit EndOfFile(const string& m);
    const char* what() const noexcept override;
};

class ParseError : public exception {
    string msg;
public:
    explicit ParseError(const string& m);
    const char* what() const noexcept override;
};

constexpr uint64_t START_LINE = 4;

class EventReader {
private:
    uint64_t lineNum_;
    uint64_t maxTableId_;
public:
    EventReader(uint64_t ln = START_LINE, uint64_t maxTableId_ = 1e9);
    string readEvent() const;

    tuple<pair<string, uint64_t>, uint64_t, string, optional<uint64_t>> parseEvent(const string& s);

    bool checkClientName(const string& name) const;

    pair<uint64_t, bool> toInt(const std::string& s);

    uint64_t getTimeInSecs(const string& str) const;

    std::vector<std::string> split(const std::string& str, char delimiter) const;
};
