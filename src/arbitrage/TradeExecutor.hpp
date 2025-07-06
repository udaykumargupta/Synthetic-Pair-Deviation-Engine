#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "arbitrage/ArbitrageOpportunity.hpp"

struct ExecutedTrade {
    std::string symbol;
    std::string buyExchange;
    std::string sellExchange;
    double buyPrice;
    double sellPrice;
    double capitalUsed;
    double profit;
    std::chrono::system_clock::time_point timestamp;
};

class TradeExecutor {
public:
    static void executeTrade(const ArbitrageOpportunity& opp);
    static double getTotalProfit();
    static int getTradeCount();
    static const std::vector<ExecutedTrade>& getTradeHistory();

    static void printPnLSummary();
    static void writeTradeHistoryToCSV(const std::string& filename);

private:
    static std::vector<ExecutedTrade> tradeHistory;
    static double totalProfit;
};
