#include "arbitrage/LiquidityAnalyzer.hpp"
#include <algorithm>
#include<iostream>

double LiquidityAnalyzer::estimateSlippage(const OrderBookUpdate& data, double tradeSizeUSDT) {
    double mid = (data.bestBid + data.bestAsk) / 2.0;
    double spread = data.bestAsk - data.bestBid;

    // Assume basic model: slippage is proportional to trade size and spread
    double impactFactor = std::min(tradeSizeUSDT / 10000.0, 1.0);  // Cap slippage influence
    return spread * impactFactor;  // USDT
}

bool LiquidityAnalyzer::isLiquiditySufficient(const OrderBookUpdate& book, double capital) {
    double mid = (book.bestBid + book.bestAsk) / 2.0;
    double requiredQty = capital / mid;

    // Simulate depth on both sides
    double bidDepth = 0.0, askDepth = 0.0;
    double bidPriceLimit = book.bestBid * 0.995; // 0.5% lower
    double askPriceLimit = book.bestAsk * 1.005; // 0.5% higher

    for (const auto& [price, qty] : book.bids) {
        if (price < bidPriceLimit) break;
        bidDepth += qty;
    }

    for (const auto& [price, qty] : book.asks) {
        if (price > askPriceLimit) break;
        askDepth += qty;
    }

    bool sufficient = (bidDepth >= requiredQty) && (askDepth >= requiredQty);
    if (!sufficient) {
        std::cout << "⚠️ Liquidity insufficient: "
                  << "Required Qty: " << requiredQty
                  << ", Bid Depth: " << bidDepth
                  << ", Ask Depth: " << askDepth << "\n";
    }

    return sufficient;
}
