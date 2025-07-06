#pragma once
#include <string>
#include <unordered_map>
#include <deque>
#include <cmath>
#include <iostream>

class CorrelationAnalyzer {
private:
    static inline std::unordered_map<std::string, std::deque<double>> priceHistory;
    static constexpr size_t WINDOW_SIZE = 100;

    static double computeCorrelation(const std::deque<double>& x, const std::deque<double>& y) {
        if (x.size() != y.size() || x.size() < 2) return 0.0;

        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
        size_t n = x.size();

        for (size_t i = 0; i < n; ++i) {
            sumX += x[i];
            sumY += y[i];
            sumXY += x[i] * y[i];
            sumX2 += x[i] * x[i];
            sumY2 += y[i] * y[i];
        }

        double numerator = (n * sumXY) - (sumX * sumY);
        double denominator = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
        if (denominator == 0.0) return 0.0;

        return numerator / denominator;
    }

public:
    static void updatePrice(const std::string& symbol, double price) {
        auto& history = priceHistory[symbol];
        history.push_back(price);
        if (history.size() > WINDOW_SIZE) history.pop_front();
    }

    static double getCorrelation(const std::string& symbolA, const std::string& symbolB) {
        const auto& histA = priceHistory[symbolA];
        const auto& histB = priceHistory[symbolB];
        return computeCorrelation(histA, histB);
    }

    static void displayAlertIfDiverging(const std::string& symbolA, const std::string& symbolB, double threshold = 0.85) {
        double corr = getCorrelation(symbolA, symbolB);
        if (std::abs(corr) >= threshold) return; // acceptable

        std::cout << "⚠️ Correlation Alert: " << symbolA << " & " << symbolB
                  << " correlation dropped to " << corr << "\n";
    }
};
