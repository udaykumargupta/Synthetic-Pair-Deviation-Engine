#pragma once
#include <string>
#include <chrono>

enum class OptionType {
    CALL,
    PUT
};

class OptionPricer {
public:
    // Black-Scholes Model
    static double blackScholesPrice(
        OptionType type,
        double S,     // Spot price
        double K,     // Strike price
        double T,     // Time to expiry (in years)
        double r,     // Risk-free rate
        double sigma  // Volatility
    );

    static double computeImpliedVolatility(
        OptionType type,
        double marketPrice,
        double S,
        double K,
        double T,
        double r
    );
};
