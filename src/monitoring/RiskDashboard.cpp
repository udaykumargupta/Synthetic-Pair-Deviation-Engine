#include "monitoring/RiskDashboard.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

void RiskDashboard::displayFundingImpact(const std::string& symbol, double fundingRate, double capital) {
    double impact = fundingRate * capital;
    std::cout << "💡 Funding Rate Impact [" << symbol << "]: "
              << std::fixed << std::setprecision(4) << fundingRate
              << " → Cost: " << std::fixed << std::setprecision(2) << impact << " USDT\n";
}

void RiskDashboard::displayLiquidityAlert(const std::string& symbol, const OrderBookUpdate& book, double requiredQty) {
    if (book.bestBidQty < requiredQty || book.bestAskQty < requiredQty) {
        std::cout << "⚠️ Liquidity Warning for " << symbol << ": Insufficient depth for "
                  << requiredQty << " units.\n";
    }
}

void RiskDashboard::displayBasisRisk(const std::string& symbol, double realPrice, double syntheticPrice) {
    double basis = syntheticPrice - realPrice;
    double basisPct = (basis / realPrice) * 100.0;
    std::cout << "📉 Basis Risk [" << symbol << "]: "
              << std::fixed << std::setprecision(2)
              << basis << " USDT (" << basisPct << "%)\n";
}
