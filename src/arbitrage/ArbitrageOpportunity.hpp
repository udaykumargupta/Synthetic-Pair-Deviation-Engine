#pragma once
#include <string>
#include <sstream>
#include "exchange/MarketDataTypes.hpp"
 
struct ArbitrageOpportunity {
    std::string symbol;
    std::string longExchange;
    std::string shortExchange;
    std::string strategyType="unknown strategy"; 
    

    double longPrice = 0.0;
    double shortPrice = 0.0;
    double profitPercentage = 0.0;
    double capital = 0.0;
    OrderBookUpdate longBook;
    OrderBookUpdate shortBook;
    std::string describe() const {
        std::ostringstream oss;
        oss << "💰 Arbitrage Opportunity: [" << symbol << "]\n"
            << "➡️ Buy from: " << longExchange << " at " << longPrice << "\n"
            << "⬅️ Sell to: " << shortExchange << " at " << shortPrice << "\n"
            << "📈 Profit: " << profitPercentage << "%\n"
            << "💵 Capital Required: " << capital << " USDT\n"
            << "🔍 Strategy: " << strategyType << "\n";

        return oss.str();
    }
};
