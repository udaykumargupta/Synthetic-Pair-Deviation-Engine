#pragma once
#include "exchange/MarketDataTypes.hpp"
#include <algorithm>  // for std::min

class ArbitrageLegOptimizer {
public:
    static double computeCapitalLimit(const OrderBookUpdate& longLeg, const OrderBookUpdate& shortLeg, double maxCapital) {
        // Defensive checks for invalid inputs
        if (longLeg.bestAsk <= 0 || longLeg.bestAskQty <= 0 || 
            shortLeg.bestBid <= 0 || shortLeg.bestBidQty <= 0) {
            return 0.0;
        }

        // Calculate minimum tradable quantity
        double tradeQty = std::min(longLeg.bestAskQty, shortLeg.bestBidQty);

        // Compute cost and return
        double longCost = tradeQty * longLeg.bestAsk;
        double shortReturn = tradeQty * shortLeg.bestBid;

        // Final capital required is the smallest of long cost, short return, and maxCapital
        return std::min({longCost, shortReturn, maxCapital});
    }
};
