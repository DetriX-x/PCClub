#pragma once

#include "EventReader.h"
#include "Event.h"
#include "Club.h"

struct EventFactory {
    EventReader er_;

    EventFactory(EventReader& er): er_{er} {}
    
    Club::IEvent create() {
        auto s = er_.readEvent();
        auto [pair, id, clientName, tableId] = er_.parseEvent(s);
        auto [sTime, time] = pair;

        if (id == static_cast<decltype(id)>(EventType::IncomingClientArraved)) {
            return IncomingClientArraved(time, clientName);
        } else if (id == static_cast<decltype(id)>(EventType::IncomingClientSet)) {
            return IncomingClientSet{time, clientName, tableId.value()};
        } else if (id == static_cast<decltype(id)>(EventType::IncomingClientWaiting)) {
            return IncomingClientWaiting{time, clientName};
        } else if (id == static_cast<decltype(id)>(EventType::IncomingClientLeft)) {
            return IncomingClientLeft{time, clientName};
        }
        return IncomingClientArraved{0, ""};
    }
};
