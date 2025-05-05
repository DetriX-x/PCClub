#include "EventReader.h"

EndOfFile::EndOfFile(const string& m) : msg(m) {}

const char* EndOfFile::what() const noexcept {
    return msg.c_str();
}

ParseError::ParseError(const string& m) : msg(m) {}

const char* ParseError::what() const noexcept {
    return msg.c_str();
}

EventReader::EventReader(std::ifstream& in, uint64_t ln, uint64_t maxTableId)
    : in_{in}, lineNum_(ln), maxTableId_(maxTableId) {}

string EventReader::readEvent() const {
    string s;
    getline(in_, s);
    if (!in_) {
        throw EndOfFile("End of file");
    }
    return s;
}

tuple<pair<string, uint64_t>, uint64_t, string, optional<uint64_t>> EventReader::parseEvent(const string& s) {
    auto tokens = split(s, ' ');
    if (tokens.size() < 3) {
        throw ParseError(to_string(lineNum_));
    }

    string sTime = tokens[0];
    uint64_t time = getTimeInSecs(sTime);
    auto [eventId, ok] = toInt(tokens[1]);
    if (!ok) {
        throw ParseError(to_string(lineNum_));
    }

    string clientName = tokens[2];
    if (!checkClientName(clientName)) {
        throw ParseError(to_string(lineNum_));
    }

    uint64_t tableId{};
    optional<uint64_t> opt;
    size_t targetSize = 3;
    if (eventId == 2) {
        ++targetSize;
        if (tokens.size() != targetSize) {
            throw ParseError(to_string(lineNum_));
        }
        bool ok{false};
        std::tie(tableId, ok) = toInt(tokens[3]);
        if (tableId > maxTableId_ || !ok) {
            throw ParseError(to_string(lineNum_));
        }
        opt = tableId;
    }
    if (tokens.size() != targetSize) {
        throw ParseError(to_string(lineNum_));
    }
    ++lineNum_;
    return {{sTime, time}, eventId, clientName, opt};
}

bool EventReader::checkClientName(const string& name) const {
    for (auto ch : name) {
        if (!(std::islower(ch) || std::isdigit(ch) || ch == '_' || ch == '-')) {
            return false;
        }
    }
    return true;
}

pair<uint64_t, bool> EventReader::toInt(const std::string& s) {
    uint64_t res{};
    try {
        size_t pos;
        res = std::stoull(s, &pos);
        if (pos != s.size() || !res) {
            return {0, false};
        }
    } catch (...) {
        return {0, false};
    }
    return {res, true};
}

uint64_t EventReader::getTimeInSecs(const string& str) const {
    stringstream ss{str};
    string sHours{}, sMinutes{};
    getline(ss, sHours, ':');
    if (!ss || sHours.size() != 2) {
        throw ParseError(to_string(lineNum_));
    }
    getline(ss, sMinutes, '\n');
    if (!ss || sMinutes.size() != 2) {
        throw ParseError(to_string(lineNum_));
    }

    int hours{}, minutes{};
    try {
        hours = stoull(sHours);
        minutes = stoull(sMinutes);
    } catch (...) {
        throw ParseError(to_string(lineNum_));
    }

    if (hours >= 24 || minutes >= 60) {
        throw ParseError(to_string(lineNum_));
    }

    return hours * 3600 + minutes * 60;
}

std::vector<std::string> EventReader::split(const std::string& str, char delimiter) const {
    if (str.back() == ' ') {
        return {};
    }
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}
