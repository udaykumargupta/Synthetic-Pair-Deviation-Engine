#pragma once
#include "OptionPricer.hpp"
#include <iostream>

inline void checkVolatilityArbitrage() {
    // Simulated data for demonstration
    double spotPrice = 30000.0;         // BTC/USDT
    double strikePrice = 31000.0;
    double marketOptionPrice = 1200.0;  // Real market price of the option
    double timeToExpiry = 14.0 / 365.0; // 14 days
    double riskFreeRate = 0.03;
    OptionType type = OptionType::CALL;

    // 1. Compute theoretical price with assumed volatility
    double assumedVol = 0.6;
    double theoreticalPrice = OptionPricer::blackScholesPrice(
        type, spotPrice, strikePrice, timeToExpiry, riskFreeRate, assumedVol
    );

    // 2. Implied volatility
    double impliedVol = OptionPricer::computeImpliedVolatility(
        type, marketOptionPrice, spotPrice, strikePrice, timeToExpiry, riskFreeRate
    );

    double diff = marketOptionPrice - theoreticalPrice;

    std::cout << "\n📊 Volatility Arbitrage Check\n";
    std::cout << "→ Market Price: " << marketOptionPrice << "\n";
    std::cout << "→ Theoretical Price (σ=" << assumedVol << "): " << theoreticalPrice << "\n";
    std::cout << "→ Implied Volatility: " << impliedVol << "\n";

    if (std::abs(diff) > 100.0) {
        std::cout << "🚨 Arbitrage Detected! Mispricing: " << diff << " USDT\n";
    } else {
        std::cout << "✅ No significant arbitrage found.\n";
    }
}
