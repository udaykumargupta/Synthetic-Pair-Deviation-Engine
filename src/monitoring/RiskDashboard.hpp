#pragma once
#include "exchange/MarketDataTypes.hpp"
#include <string>

class RiskDashboard {
public:
    static void displayFundingImpact(const std::string& symbol, double fundingRate, double capital);
    static void displayLiquidityAlert(const std::string& symbol, const OrderBookUpdate& book, double requiredQty);
    static void displayBasisRisk(const std::string& symbol, double realPrice, double syntheticPrice);
};
