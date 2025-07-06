#pragma once
#include <vector>
#include <algorithm>
#include <iostream>

class VaREstimator {
public:
    static void addPnL(double pnl);
    static double computeHistoricalVaR(double confidenceLevel = 0.95);
    static void printVaRReport();

private:
    static inline std::vector<double> pnlHistory = {};
    static constexpr size_t MAX_HISTORY = 1000; // limit memory usage
};
