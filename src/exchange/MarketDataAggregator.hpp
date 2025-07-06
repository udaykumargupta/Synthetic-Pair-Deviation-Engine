#pragma once

#include "MarketDataTypes.hpp"
#include "arbitrage/SyntheticInstrumentCalculator.hpp" // For SyntheticInstrument
#include <unordered_map>
#include <string>
#include <mutex>
#include <optional>

struct FundingData {
    double markPrice;
    double fundingRate;
};

class MarketDataAggregator {
public:
    void update(const std::string& exchange, const OrderBookUpdate& update);
    void updateFundingAndMark(const std::string& exchange, double mark, double funding);
    std::optional<FundingData> getFundingData(const std::string& exchange) const;

    const std::unordered_map<std::string, OrderBookUpdate>& getLatestUpdates() const;
    const std::unordered_map<std::string, SyntheticInstrument>& getSyntheticData() const;

    void updateSynthetic(const std::string& name, const SyntheticInstrument& synthetic);

    void printSnapshot();

private:
    mutable std::mutex dataMutex;

    std::unordered_map<std::string, OrderBookUpdate> exchangeData;
    std::unordered_map<std::string, FundingData> fundingInfo;
    std::unordered_map<std::string, SyntheticInstrument> syntheticData;
};
