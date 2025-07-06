#pragma once
#include "exchange/MarketDataTypes.hpp"
#include "ArbitrageOpportunity.hpp"

struct SyntheticInstrument {
    std::string type;
    std::string symbol;
    double price;
    std::string legA;
    std::string legB;
};

class SyntheticInstrumentCalculator {
public:
    static SyntheticInstrument computeSyntheticSpot(const OrderBookUpdate& spotData, double leverage, double fundingRate);

    static SyntheticInstrument computeSyntheticFuture_CarryModel(const OrderBookUpdate& spotData, double costOfCarry, double timeToExpiryInYears);

    static SyntheticInstrument computeSyntheticFuture_FundingModel(const OrderBookUpdate& spotData, double fundingRate, double timeWindow);

    static double computeMispricing(double realPrice, double syntheticPrice);

static ArbitrageOpportunity evaluateArbitrage(
    const std::string& symbol,
    const std::string& realExchange,
    const std::string& syntheticExchange,
    double realPrice,
    double syntheticPrice,
    double minProfitThreshold,
    double capital,
    const OrderBookUpdate& realOrderBook,
    const OrderBookUpdate& syntheticOrderBook
);

};
