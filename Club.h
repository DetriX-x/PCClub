#pragma once

#include <memory>

#include "Event.h"
#include "Manager.h"

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

    using Events = std::vector<IEvent>;
    Events events_;

public:
    Manager manager_;

    Club(uint64_t countOfTabels, uint64_t startTime, uint64_t endTime, uint64_t price)
        : manager_(countOfTabels, startTime, endTime, price) {}

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
        if (event.time_ >= manager_.endTime_ || event.time_ < manager_.startTime_) {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.notOpenYetError()} << '\n';
            return;
        }
        auto it = manager_.clients_.find(event.clientName_);
        if (it != std::end(manager_.clients_)) {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.youShallNotPassError()} << '\n';
        } else {
            manager_.clients_.insert(it, {event.clientName_, {0, manager_.clientsQueue_.end()}});
        }
    }

    void process(IncomingClientSet event) {
        std::cout << event << '\n';
        if (auto it = manager_.clients_.find(event.clientName_); it == std::end(manager_.clients_)) {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.clientUnknownError()} << '\n';
              return;
        } 
        manager_.setClientToTable(event.time_, event.clientName_, event.tableId_, event.placeIsBusyError());
    }

    void process(IncomingClientWaiting event) {
        std::cout << event << '\n';
        if (manager_.busyTables_.size() != manager_.countOfTables_) {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.iCanWaitNoLonger()} << '\n';
            return;
        }
        if (manager_.clientsQueue_.size() > manager_.countOfTables_) {
            std::cout << OutgoingClientLeft{event.time_, event.clientName_} << '\n';
            manager_.sendOffClient(manager_.clients_.find(event.clientName_), event.time_);
            return;
        }
        manager_.clientsQueue_.push_back(event.clientName_);
        manager_.clients_[event.clientName_].second = std::prev(manager_.clientsQueue_.end());
    }

    void process(IncomingClientLeft event) {
        std::cout << event << '\n';
        if (auto clientIt = manager_.clients_.find(event.clientName_); clientIt != std::end(manager_.clients_)) {
            manager_.sendOffClient(clientIt, event.time_);
        } else {
            std::cout << OutgoingClientError{
                event.time_
              , event.clientName_
              , event.clientUnknownError()} << '\n';
        }
    }
};
