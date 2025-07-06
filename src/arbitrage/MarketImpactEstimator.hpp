// 📄 src/arbitrage/MarketImpactEstimator.hpp
#pragma once
#include "exchange/MarketDataTypes.hpp"
#include <cmath>
#include <iostream>

class MarketImpactEstimator {
public:
    static double estimateSlippage(const OrderBookUpdate& book, double orderSizeUSD, double aggressiveness = 0.2) {
        double depth = book.bestBidQty * book.bestBid + book.bestAskQty * book.bestAsk;
        if (depth <= 0.0) return 0.0;
        double impact = aggressiveness * (orderSizeUSD / depth);
        return std::min(impact * 100.0, 5.0); // cap at 5%
    }

    static void logImpactEstimate(const std::string& exchange, const OrderBookUpdate& book, double orderSizeUSD) {
        double slip = estimateSlippage(book, orderSizeUSD);
        std::cout << "📉 Estimated Slippage on " << exchange << " for $" << orderSizeUSD << ": ~" << slip << "%\n";
    }
};
