#pragma once
#include "exchange/MarketDataTypes.hpp"
#include "arbitrage/ArbitrageOpportunity.hpp"
#include <cmath>

class PositionManager {
public:
    // Risk factor: 0.01 = 1% capital risk per trade
    static double computePositionSize(double capital, double entryPrice, double riskFactor = 0.01) {
        return (capital * riskFactor) / entryPrice;
    }

    // Stop-loss at -1%, take-profit at +1.5% (can be adjusted)
    static bool shouldStopLoss(double entryPrice, double currentPrice, double threshold = 0.01) {
        double change = (currentPrice - entryPrice) / entryPrice;
        return change <= -threshold;
    }

    static bool shouldTakeProfit(double entryPrice, double currentPrice, double threshold = 0.015) {
        double change = (currentPrice - entryPrice) / entryPrice;
        return change >= threshold;
    }
};
