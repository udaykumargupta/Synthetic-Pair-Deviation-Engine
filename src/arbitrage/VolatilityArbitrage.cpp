#include "arbitrage/VolatilityArbitrage.hpp"
#include <iostream>
#include <cmath>

namespace VolatilityArbitrage {

    // Black-Scholes pricing (Call Option)
    double blackScholesCall(double S, double K, double T, double r, double sigma) {
        double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
        double d2 = d1 - sigma * std::sqrt(T);

        auto norm_cdf = [](double x) {
            return 0.5 * std::erfc(-x / std::sqrt(2));
        };

        return S * norm_cdf(d1) - K * std::exp(-r * T) * norm_cdf(d2);
    }

    void checkVolatilityArbitrage(MarketDataAggregator& aggregator) {
        const auto& latestUpdates = aggregator.getLatestUpdates();

        if (!latestUpdates.count("OKX")) return;
        const auto& okxSpot = latestUpdates.at("OKX");

        double spotPrice = (okxSpot.bestBid + okxSpot.bestAsk) / 2.0;

        // Placeholder parameters (these should eventually come from the options feed)
        double strike = spotPrice * 1.05;      // 5% OTM Call
        double timeToExpiry = 7.0 / 365.0;      // 7 days
        double interestRate = 0.02;             // 2% annual
        double impliedVol = 0.65;               // Sample IV (to be fetched later)

        double theoPrice = blackScholesCall(spotPrice, strike, timeToExpiry, interestRate, impliedVol);
        double marketOptionPrice = 150.0; // Mock: replace with actual quote from options feed

        double mispricing = (marketOptionPrice - theoPrice) / theoPrice * 100.0;

        std::cout << "📈 Volatility Arbitrage:\n";
        std::cout << "    Spot: " << spotPrice << ", Strike: " << strike << ", IV: " << impliedVol << "\n";
        std::cout << "    Theoretical Price: " << theoPrice << ", Market Price: " << marketOptionPrice << "\n";
        std::cout << "    ≡ Mispricing (IV arb): " << mispricing << "%\n";

        if (std::abs(mispricing) > 5.0) {
            std::cout << "🚨 Potential Volatility Arbitrage Opportunity Found!\n";
            // You could trigger a mock trade here using TradeExecutor if needed
        }
    }

}
