#include "arbitrage/RiskManager.hpp"
#include "arbitrage/LiquidityAnalyzer.hpp"
#include <cmath> 
#include <iostream>

bool RiskManager::isRiskAcceptable(const ArbitrageOpportunity& opportunity, const OrderBookUpdate& orderBook) {
    if (!LiquidityAnalyzer::isLiquiditySufficient(orderBook, opportunity.capital)) {
        std::cout << "🚫 Rejected due to poor liquidity\n";
        return false;
    }

    if (opportunity.capital > MAX_CAPITAL) {
        std::cout << "🚫 Rejected: Capital exceeds max allowed limit (" << MAX_CAPITAL << ")\n";
        return false;
    }

    if (std::abs(opportunity.profitPercentage) < MIN_PROFIT_PERCENT) {
        std::cout << "🚫 Rejected: Profit % (" << opportunity.profitPercentage << ") below threshold (" << MIN_PROFIT_PERCENT << ")\n";
        return false;
    }

    return true;
}

