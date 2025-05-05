#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <string>
#include <memory>
#include <cmath>
#include <algorithm>

#include "Event.h"

struct EventFactory;

template <typename ClubT, typename T>
concept IsProcessibleByClub = requires(ClubT& club, T t) {
    {club.process(t)} -> std::same_as<void>;
};

class Club {
private:
    friend struct EventFactory;
    struct IEvent {
        struct EventConcept {
            virtual ~EventConcept() {};
            virtual void process(Club& club) const = 0;
            virtual std::unique_ptr<EventConcept> clone() const = 0;
        };

        template <typename T> struct EventModel final: EventConcept {
            T object_;
            EventModel(const T& value): object_{value} {}

            void process(Club& club) const override {
                club.process(object_); // mixing dynamic and static polymorphism
            }

            std::unique_ptr<EventConcept> clone() const override {
                return std::make_unique<EventModel>(*this);
            }
        };

        std::unique_ptr<EventConcept> pimpl_;

        template <typename T>
        IEvent(const T& x)
            : pimpl_{new EventModel<T>(x)} {
            static_assert(IsProcessibleByClub<Club, T>);
        }

        IEvent(const IEvent& s)
            : pimpl_{s.pimpl_->clone()} {
        }

        IEvent(IEvent&& s)
            : pimpl_{std::move(s.pimpl_)} {
        }

        IEvent& operator=(const IEvent& s) {
            pimpl_ = s.pimpl_->clone();
            return *this;
        }

        IEvent& operator=(IEvent&& s) {
            pimpl_ = std::move(s.pimpl_);
            return *this;
        }
    };
    
private:
    struct Table {
        uint64_t startTime_ = 0;
        uint64_t earnings_ = 0;
        uint64_t busyTime_ = 0;
    };

    uint64_t countOfTables_;
    uint64_t startTime_;
    uint64_t endTime_;
    uint64_t price_;

    std::vector<Table> tables_;
    std::unordered_set<uint64_t> busyTables_;
    std::list<std::string> clientsQueue_;
    using QueueIterator = decltype(clientsQueue_)::iterator;
    std::unordered_map<std::string, std::pair<uint64_t, QueueIterator>> clients_; // name -> [id of table, iterator_to_queue_pos]
    
    using Events = std::vector<IEvent>;
    Events events_;

    void calcEarning(Table& table, uint64_t endTime) const {
        table.earnings_ += std::ceil((static_cast<double>(endTime) - table.startTime_) / 3600.) * price_;
    }

    void calcBusyTime(Table& table, uint64_t endTime) const {
        table.busyTime_ += endTime - table.startTime_;
    }
public:
    Club(uint64_t countOfTabels, uint64_t startTime, uint64_t endTime, uint64_t price)
    : countOfTables_{countOfTabels}
    , startTime_{startTime}
    , endTime_{endTime}
    , price_{price}
    , tables_(countOfTabels + 1) {}

    void addEvent(const IEvent& event) {
        events_.push_back(event);
    }

    void processAll() {
        for (auto& e: events_) {
            e.pimpl_.get()->process(*this);
        }
    }

    void process(IncomingClientArraved event) {
        std::cout << event << '\n';
        if (event.time_ >= endTime_ || event.time_ < startTime_) {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.notOpenYetError()} << '\n';
            return;
        }
        auto it = clients_.find(event.clientName_);
        if (it != std::end(clients_)) {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.youShallNotPassError()} << '\n';
        } else {
            clients_.insert(it, {event.clientName_, {0, clientsQueue_.end()}});
        }
    }

    void process(IncomingClientSet event) {
        std::cout << event << '\n';
        if (auto it = clients_.find(event.clientName_); it == std::end(clients_)) {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.clientUnknownError()} << '\n';
              return;
        } 
        setClientToTable(event.time_, event.clientName_, event.tableId_, event.placeIsBusyError());
    }

    void setClientToTable(uint64_t time, const std::string& clientName, uint64_t tableId, const std::string& msg = "") {
        auto it = busyTables_.find(tableId);
        if (it != std::end(busyTables_)) {
            std::cout << OutgoingClientError{
                time
              , clientName
              , msg} << '\n';
            return;
        } 
        [[maybe_unused]] auto [id, p] = clients_[clientName];
        if (id != 0) {
            calcEarning(tables_[id], time);
            calcBusyTime(tables_[id], time);
            busyTables_.erase(id);
        }
        busyTables_.insert(it, tableId);
        tables_[tableId].startTime_ = time;
        clients_[clientName] = {tableId, clientsQueue_.end()};
    }

    void process(IncomingClientWaiting event) {
        std::cout << event << '\n';
        if (busyTables_.size() != countOfTables_) {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.iCanWaitNoLonger()} << '\n';
            return;
        }
        if (clientsQueue_.size() > countOfTables_) {
            std::cout << OutgoingClientLeft{event.time_, event.clientName_} << '\n';
            sendOffClient(clients_.find(event.clientName_), event.time_);
            return;
        }
        clientsQueue_.push_back(event.clientName_);
        clients_[event.clientName_].second = std::prev(clientsQueue_.end());
    }

    void process(IncomingClientLeft event) {
        std::cout << event << '\n';
        if (auto clientIt = clients_.find(event.clientName_); clientIt != std::end(clients_)) {
            sendOffClient(clientIt, event.time_);
        } else {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.clientUnknownError()} << '\n';
        }
    }

    void sendOffClient(decltype(clients_)::iterator clientIt, uint64_t time) {
        auto [name, p] = *clientIt;
        auto [tableId, qPos] = p;
        if (qPos != clientsQueue_.end()) {
            // client is in queue and is leaving now
            clientsQueue_.erase(qPos);
        }
        if (auto tableIt = busyTables_.find(tableId); tableIt != std::end(busyTables_)) {
            calcEarning(tables_[tableId], time);
            calcBusyTime(tables_[tableId], time);
            if (clientsQueue_.size()) {
                auto nextClient = clientsQueue_.front();
                std::cout << OutgoingClientSet{time, nextClient, tableId} << '\n';
                busyTables_.erase(tableId);
                setClientToTable(time, nextClient, tableId, "Unexpected error!");
                clientsQueue_.pop_front();
            } else {
                busyTables_.erase(tableIt);
            }
        }
        clients_.erase(clientIt);
    }

    void kickOutAll() {
        std::vector<std::string> temp;
        temp.reserve(clients_.size() + clientsQueue_.size());
        for (auto& [name, p]: clients_) {
            auto [tableId, qPos] = p;
            temp.push_back(name);
            if (auto tableIt = busyTables_.find(tableId); tableIt != std::end(busyTables_)) {
                calcEarning(tables_[tableId], endTime_);
                calcBusyTime(tables_[tableId], endTime_);
            }
        }

        clients_.clear();
        busyTables_.clear();
        sort(temp.begin(), temp.end());

        for (auto& name: temp) {
            std::cout << OutgoingClientLeft{endTime_, name} << '\n';
        }
    }

    void showStats() const {
        for (size_t i = 1; i != tables_.size(); ++i) {
            std::cout << i << ' ' << tables_[i].earnings_ << ' ' << timeToString(tables_[i].busyTime_) << '\n';
        }
    }
};
