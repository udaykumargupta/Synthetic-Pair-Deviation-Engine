#include "MarketDataAggregator.hpp"
#include <iostream>
#include <iomanip>

void MarketDataAggregator::update(const std::string& exchange, const OrderBookUpdate& update) {
    std::lock_guard<std::mutex> lock(dataMutex);
    exchangeData[exchange] = update;
}

void MarketDataAggregator::printSnapshot() {
    std::lock_guard<std::mutex> lock(dataMutex);

    std::cout << "\n=== Market Snapshot ===\n";
    for (const auto& [exchange, update] : exchangeData) {
        std::cout << exchange << "  - " << update.symbol 
                  << " | Bid: " << update.bestBid 
                  << " | Ask: " << update.bestAsk << "\n";

        auto it = fundingInfo.find(exchange);
        if (it != fundingInfo.end()) {
            std::cout << std::fixed << std::setprecision(10);
            std::cout << "   ↳ Mark: " << it->second.markPrice 
                      << ", Funding: " << it->second.fundingRate << "\n";
        }
    }

    std::cout << "\n=== Synthetic Instruments ===\n";
    for (const auto& [name, syn] : syntheticData) {
        std::cout << "🔹 " << syn.type << " [" << name << "] - Price: " << syn.price 
                  << " (LegA: " << syn.legA << ", LegB: " << syn.legB << ")\n";
    }
}



const std::unordered_map<std::string, OrderBookUpdate>& MarketDataAggregator::getLatestUpdates() const {
    return exchangeData;
}
void MarketDataAggregator::updateFundingAndMark(const std::string& exchange, double mark, double funding) {
    std::lock_guard<std::mutex> lock(dataMutex);
    fundingInfo[exchange] = {mark, funding};
}

std::optional<FundingData> MarketDataAggregator::getFundingData(const std::string& exchange) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    auto it = fundingInfo.find(exchange);
    if (it != fundingInfo.end()) {
        return it->second;
    }
    return std::nullopt;
}

// MarketDataAggregator.cpp
void MarketDataAggregator::updateSynthetic(const std::string& name, const SyntheticInstrument& synthetic) {
    std::lock_guard<std::mutex> lock(dataMutex);
    syntheticData[name] = synthetic;
}

const std::unordered_map<std::string, SyntheticInstrument>& MarketDataAggregator::getSyntheticData() const {
    return syntheticData;
}
