#pragma once
#include "arbitrage/ArbitrageOpportunity.hpp"
#include "exchange/MarketDataTypes.hpp"

class RiskManager {
public:
    // Evaluates if the arbitrage opportunity is within acceptable risk limits
    static bool isRiskAcceptable(const ArbitrageOpportunity& opportunity, const OrderBookUpdate& orderBook);


private:
    // Risk thresholds (can later be made configurable)
    static constexpr double MAX_CAPITAL = 20000.0;        // in USDT
    static constexpr double MIN_PROFIT_PERCENT = 0.01;     // in %

    // Additional checks can be added later (e.g., liquidity, volatility, etc.)
};
