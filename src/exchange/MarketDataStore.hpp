#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include <optional>
#include "MarketDataTypes.hpp"

class MarketDataStore {
public:
    void update(const std::string& exchange, const OrderBookUpdate& update);

    std::optional<OrderBookUpdate> get(const std::string& exchange, const std::string& symbol) const;

    std::unordered_map<std::string, std::unordered_map<std::string, OrderBookUpdate>> snapshot() const;

private:
    mutable std::mutex mtx;
    std::unordered_map<std::string, std::unordered_map<std::string, OrderBookUpdate>> data;
};
