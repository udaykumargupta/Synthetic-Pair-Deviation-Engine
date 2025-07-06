#include "monitoring/VaREstimator.hpp"
#include <iomanip>

void VaREstimator::addPnL(double pnl) {
    pnlHistory.push_back(pnl);
    if (pnlHistory.size() > MAX_HISTORY) {
        pnlHistory.erase(pnlHistory.begin());
    }
}

double VaREstimator::computeHistoricalVaR(double confidenceLevel) {
    if (pnlHistory.empty()) return 0.0;

    std::vector<double> losses;
    for (double pnl : pnlHistory) {
        if (pnl < 0) losses.push_back(-pnl); // treat as loss
    }

    if (losses.empty()) return 0.0;

    std::sort(losses.begin(), losses.end());
    size_t index = static_cast<size_t>((1.0 - confidenceLevel) * losses.size());

    return losses[index];
}

void VaREstimator::printVaRReport() {
    double var95 = computeHistoricalVaR(0.95);
    double var99 = computeHistoricalVaR(0.99);

    std::cout << "\n📉 === VaR REPORT ===\n";
    std::cout << "VaR (95%): $" << std::fixed << std::setprecision(2) << var95 << "\n";
    std::cout << "VaR (99%): $" << std::fixed << std::setprecision(2) << var99 << "\n";
}
