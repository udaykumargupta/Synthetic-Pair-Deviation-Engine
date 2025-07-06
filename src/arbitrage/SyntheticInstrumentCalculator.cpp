#include "arbitrage/SyntheticInstrumentCalculator.hpp"
#include <iostream>
#include <cmath>

SyntheticInstrument SyntheticInstrumentCalculator::computeSyntheticSpot(const OrderBookUpdate& spotData, double leverage, double fundingRate) {
    double mid = (spotData.bestBid + spotData.bestAsk) / 2.0;
    double syntheticPrice = mid * (1 + fundingRate * leverage);

    return {
        .type = "Synthetic Spot",
        .symbol = spotData.symbol,
        .price = syntheticPrice,
        .legA = spotData.symbol + "_PERP",
        .legB = "FundingAdj"
    };
}

SyntheticInstrument SyntheticInstrumentCalculator::computeSyntheticFuture_CarryModel(
    const OrderBookUpdate& spotData, double costOfCarry, double timeToExpiryInYears) {

    double mid = (spotData.bestBid + spotData.bestAsk) / 2.0;
    double syntheticPrice = mid * (1 + costOfCarry * timeToExpiryInYears);

    return {
        .type = "Synthetic Future (Carry)",
        .symbol = spotData.symbol,
        .price = syntheticPrice,
        .legA = spotData.symbol + "_SPOT",
        .legB = "CostOfCarry"
    };
}

SyntheticInstrument SyntheticInstrumentCalculator::computeSyntheticFuture_FundingModel(
    const OrderBookUpdate& spotData, double fundingRate, double timeWindow) {

    double mid = (spotData.bestBid + spotData.bestAsk) / 2.0;
    double syntheticPrice = mid * (1 + fundingRate * timeWindow);

    return {
        .type = "Synthetic Future (Funding)",
        .symbol = spotData.symbol,
        .price = syntheticPrice,
        .legA = spotData.symbol + "_SPOT",
        .legB = "FundingRate"
    };
}

double SyntheticInstrumentCalculator::computeMispricing(double realPrice, double syntheticPrice) {
    if (syntheticPrice == 0) return 0.0;
    return ((realPrice - syntheticPrice) / syntheticPrice) * 100.0;
}

ArbitrageOpportunity SyntheticInstrumentCalculator::evaluateArbitrage(
    const std::string& symbol,
    const std::string& realExchange,
    const std::string& syntheticExchange,
    double realPrice,
    double syntheticPrice,
    double minProfitThreshold,
    double capital,
    const OrderBookUpdate& realOrderBook,
    const OrderBookUpdate& syntheticOrderBook
)
 {
    ArbitrageOpportunity result;
    result.symbol = symbol;

    if (syntheticPrice > realPrice) {
    double profitPct = ((syntheticPrice - realPrice) / realPrice) * 100.0;
    if (profitPct >= minProfitThreshold) {
        result = {
            .symbol = symbol,
            .longExchange = realExchange,
            .shortExchange = syntheticExchange,
            .longPrice = realPrice,
            .shortPrice = syntheticPrice,
            .profitPercentage = profitPct,
            .capital = capital,
            .longBook = realOrderBook,
            .shortBook = syntheticOrderBook
        };

        std::cout << "\n💰 Arbitrage Opportunity Detected:\n";
        std::cout << "🔹 Symbol: " << symbol << "\n";
        std::cout << "🟢 Buy: " << result.longExchange << " at " << result.longPrice << "\n";
        std::cout << "🔴 Sell: " << result.shortExchange << " at " << result.shortPrice << "\n";
        std::cout << "📈 Profit: " << profitPct << "%\n";
        std::cout << "💵 Capital Required: " << capital << " USDT\n\n";
    }
} else if (realPrice > syntheticPrice) {
    double profitPct = ((realPrice - syntheticPrice) / syntheticPrice) * 100.0;
    if (profitPct >= minProfitThreshold) {
        result = {
            .symbol = symbol,
            .longExchange = syntheticExchange,
            .shortExchange = realExchange,
            .longPrice = syntheticPrice,
            .shortPrice = realPrice,
            .profitPercentage = profitPct,
            .capital = capital,
            .longBook = syntheticOrderBook,
            .shortBook = realOrderBook
        };
    }
}
 else if (realPrice > syntheticPrice) {
        double profitPct = ((realPrice - syntheticPrice) / syntheticPrice) * 100.0;
        if (profitPct >= minProfitThreshold) {
            result.longExchange = syntheticExchange;
            result.shortExchange = realExchange;
            result.longPrice = syntheticPrice;
            result.shortPrice = realPrice;
            result.profitPercentage = profitPct;
            result.capital = capital;
        }
    }

    return result;
}
