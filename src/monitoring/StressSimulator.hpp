#pragma once
#include "exchange/MarketDataTypes.hpp"
#include <iostream>
#include <random>

class StressSimulator {
public:
    static OrderBookUpdate simulatePriceShock(const OrderBookUpdate& original, double shockPercent) {
        OrderBookUpdate shocked = original;
        shocked.bestBid *= (1.0 - shockPercent / 100.0);
        shocked.bestAsk *= (1.0 - shockPercent / 100.0);
        std::cout << "⚠️ Price Shock Applied: " << shockPercent << "% down\n";
        return shocked;
    }

    static OrderBookUpdate simulateLiquidityDrain(const OrderBookUpdate& original) {
        OrderBookUpdate drained = original;
        drained.bidQty *= 0.1;  // Reduce liquidity to 10%
        drained.askQty *= 0.1;
        std::cout << "⚠️ Liquidity Drain Simulated\n";
        return drained;
    }

    static void simulateLatency(int msDelay) {
        std::cout << "🐢 Injected latency: " << msDelay << " ms\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(msDelay));
    }
};
