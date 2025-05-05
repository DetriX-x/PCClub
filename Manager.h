#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <string>
#include <cmath>
#include <algorithm>

#include "Event.h"

struct Manager {
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
    
    void calcEarning(Table& table, uint64_t endTime) const {
        table.earnings_ += std::ceil((static_cast<double>(endTime) - table.startTime_) / 3600.) * price_;
    }

    void calcBusyTime(Table& table, uint64_t endTime) const {
        table.busyTime_ += endTime - table.startTime_;
    }

    Manager(uint64_t countOfTabels, uint64_t startTime, uint64_t endTime, uint64_t price)
    : countOfTables_{countOfTabels}
    , startTime_{startTime}
    , endTime_{endTime}
    , price_{price}
    , tables_(countOfTabels + 1) {}


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
        temp.reserve(clients_.size());
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
