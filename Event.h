#pragma once
#include <iostream>
#include <string>
#include <cstdint>

enum class EventType {
    IncomingClientArraved = 1,
    IncomingClientSet,
    IncomingClientWaiting,
    IncomingClientLeft,
    OutgoingClientLeft = 11,
    OutgoingClientSet,
    OutgoingClientError
};

std::string timeToString(uint64_t time);

struct Event {
    uint64_t time_;
    std::string clientName_;

    Event(uint64_t time, const std::string& clientName);
};

struct IncomingClientArraved : Event {
    using Event::Event;

    std::string youShallNotPassError() const;
    std::string notOpenYetError() const;

    friend std::ostream& operator<<(std::ostream& os, const IncomingClientArraved& event);
};

struct IncomingClientSet : Event {
    uint64_t tableId_;

    IncomingClientSet(uint64_t time, const std::string& clientName, uint64_t tableId);

    std::string placeIsBusyError() const;
    std::string clientUnknownError() const;

    friend std::ostream& operator<<(std::ostream& os, const IncomingClientSet& event);
};

struct IncomingClientWaiting : Event {
    using Event::Event;

    std::string iCanWaitNoLonger() const;

    friend std::ostream& operator<<(std::ostream& os, const IncomingClientWaiting& event);
};

struct IncomingClientLeft : Event {
    using Event::Event;

    std::string clientUnknownError() const;

    friend std::ostream& operator<<(std::ostream& os, const IncomingClientLeft& event);
};

struct OutgoingClientLeft : Event {
    using Event::Event;

    friend std::ostream& operator<<(std::ostream& os, const OutgoingClientLeft& event);
};

struct OutgoingClientSet : Event {
    uint64_t tableId_;

    OutgoingClientSet(uint64_t time, const std::string& clientName, uint64_t tableId);

    friend std::ostream& operator<<(std::ostream& os, const OutgoingClientSet& event);
};

struct OutgoingClientError : Event {
    std::string error_;

    OutgoingClientError(uint64_t time, const std::string& clientName, const std::string& error);

    friend std::ostream& operator<<(std::ostream& os, const OutgoingClientError& event);
};
