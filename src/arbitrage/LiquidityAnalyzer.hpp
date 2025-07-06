#pragma once
#include "exchange/MarketDataTypes.hpp"

class LiquidityAnalyzer {
public:
    static double estimateSlippage(const OrderBookUpdate& data, double tradeSizeUSDT);
    static bool isLiquiditySufficient(const OrderBookUpdate& book, double capital);
};
