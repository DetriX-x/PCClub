#include "Event.h"

#include <iostream>
#include <iomanip>
#include <sstream>

std::string timeToString(uint64_t time) {
    int hours = time / 3600;
    int minutes = (time % 3600) / 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours
        << ":"
        << std::setw(2) << std::setfill('0') << minutes;

    return oss.str();
}

Event::Event(uint64_t time, const std::string& clientName)
    : time_{time}, clientName_{clientName} {}

std::string IncomingClientArraved::youShallNotPassError() const {
    return "YouShallNotPass";
}

std::string IncomingClientArraved::notOpenYetError() const {
    return "NotOpenYet";
}

std::ostream& operator<<(std::ostream& os, const IncomingClientArraved& event) {
    os << timeToString(event.time_) << ' ' << static_cast<int>(EventType::IncomingClientArraved) << ' ' << event.clientName_;
    return os;
}

IncomingClientSet::IncomingClientSet(uint64_t time, const std::string& clientName, uint64_t tableId)
    : Event(time, clientName), tableId_{tableId} {}

std::string IncomingClientSet::placeIsBusyError() {
    return "PlaceIsBusy";
}

std::string IncomingClientSet::clientUnknownError() const {
    return "ClientUnknown";
}

std::ostream& operator<<(std::ostream& os, const IncomingClientSet& event) {
    os << timeToString(event.time_) << ' ' << static_cast<int>(EventType::IncomingClientSet)
       << ' ' << event.clientName_ << ' ' << event.tableId_;
    return os;
}

std::string IncomingClientWaiting::iCanWaitNoLonger() const {
    return "ICanWaitNoLonger!";
}

std::ostream& operator<<(std::ostream& os, const IncomingClientWaiting& event) {
    os << timeToString(event.time_) << ' ' << static_cast<int>(EventType::IncomingClientWaiting) << ' ' << event.clientName_;
    return os;
}

std::string IncomingClientLeft::clientUnknownError() const {
    return "ClientUnknown";
}

std::ostream& operator<<(std::ostream& os, const IncomingClientLeft& event) {
    os << timeToString(event.time_) << ' ' << static_cast<int>(EventType::IncomingClientLeft) << ' ' << event.clientName_;
    return os;
}

std::ostream& operator<<(std::ostream& os, const OutgoingClientLeft& event) {
    os << timeToString(event.time_) << ' ' << static_cast<int>(EventType::OutgoingClientLeft) << ' ' << event.clientName_;
    return os;
}

OutgoingClientSet::OutgoingClientSet(uint64_t time, const std::string& clientName, uint64_t tableId)
    : Event(time, clientName), tableId_{tableId} {}

std::ostream& operator<<(std::ostream& os, const OutgoingClientSet& event) {
    os << timeToString(event.time_) << ' ' << static_cast<int>(EventType::OutgoingClientSet)
       << ' ' << event.clientName_ << ' ' << event.tableId_;
    return os;
}

OutgoingClientError::OutgoingClientError(uint64_t time, const std::string& clientName, const std::string& error)
    : Event(time, clientName), error_{error} {}

std::ostream& operator<<(std::ostream& os, const OutgoingClientError& event) {
    os << timeToString(event.time_) << ' ' << static_cast<int>(EventType::OutgoingClientError)
       << ' ' << event.error_;
    return os;
}

