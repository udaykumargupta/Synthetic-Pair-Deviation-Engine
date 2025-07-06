#include "arbitrage/options/OptionPricer.hpp"
#include <cmath>
#include <iostream>

namespace {
    double normCDF(double x) {
        return 0.5 * erfc(-x / std::sqrt(2));
    }
}

double OptionPricer::blackScholesPrice(OptionType type, double S, double K, double T, double r, double sigma) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    if (type == OptionType::CALL)
        return S * normCDF(d1) - K * std::exp(-r * T) * normCDF(d2);
    else
        return K * std::exp(-r * T) * normCDF(-d2) - S * normCDF(-d1);
}

double OptionPricer::computeImpliedVolatility(OptionType type, double marketPrice, double S, double K, double T, double r) {
    double low = 0.0001, high = 5.0, tol = 1e-5;
    double mid;

    for (int i = 0; i < 100; ++i) {
        mid = (low + high) / 2.0;
        double price = blackScholesPrice(type, S, K, T, r, mid);
        if (std::abs(price - marketPrice) < tol) break;
        if (price > marketPrice) high = mid;
        else low = mid;
    }

    return mid;
}
