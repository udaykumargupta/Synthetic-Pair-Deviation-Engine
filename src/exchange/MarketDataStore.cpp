#include "MarketDataStore.hpp"

void MarketDataStore::update(const std::string& exchange, const OrderBookUpdate& update) {
    std::lock_guard<std::mutex> lock(mtx);
    data[exchange][update.symbol] = update;
}

std::optional<OrderBookUpdate> MarketDataStore::get(const std::string& exchange, const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(exchange);
    if (it != data.end()) {
        auto jt = it->second.find(symbol);
        if (jt != it->second.end()) {
            return jt->second;
        }
    }
    return std::nullopt;
}

std::unordered_map<std::string, std::unordered_map<std::string, OrderBookUpdate>> MarketDataStore::snapshot() const {
    std::lock_guard<std::mutex> lock(mtx);
    return data;
}

