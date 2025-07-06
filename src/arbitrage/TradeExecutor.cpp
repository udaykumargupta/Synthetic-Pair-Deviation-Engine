#include "arbitrage/TradeExecutor.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "arbitrage/Risk/PositionManager.hpp"
#include <monitoring/VaREstimator.hpp>
#include "MarketImpactEstimator.hpp"

std::vector<ExecutedTrade> TradeExecutor::tradeHistory = {};
double TradeExecutor::totalProfit = 0.0;

void TradeExecutor::executeTrade(const ArbitrageOpportunity& opp) {
    if (opp.capital <= 0.0) {
        std::cerr << "❌ Trade rejected: Capital is zero or negative.\n";
        return;
    }

    double entryBuy = opp.longPrice;
    double entrySell = opp.shortPrice;

    // 🧠 Dynamic Position Sizing
    double quantity = PositionManager::computePositionSize(opp.capital, entryBuy);
    double capitalUsed = quantity * entryBuy;

    // 🔍 Simulated live market price movement (you can replace this with live feed)
    double mockMarketPrice = (entryBuy + entrySell) / 2.0;

    // 🛑 Check Stop Loss / Take Profit
    bool stopLoss = PositionManager::shouldStopLoss(entryBuy, mockMarketPrice);
    bool takeProfit = PositionManager::shouldTakeProfit(entryBuy, mockMarketPrice);

    if (stopLoss) {
        std::cout << "🔻 Stop-loss Triggered (Price dropped below threshold)\n";
        return;
    }

    if (takeProfit) {
        std::cout << "💰 Take-Profit Triggered (Profit threshold reached)\n";
    }


    MarketImpactEstimator::logImpactEstimate(opp.longExchange, opp.longBook, opp.capital);
    MarketImpactEstimator::logImpactEstimate(opp.shortExchange, opp.shortBook, opp.capital);

    double slippageBuy = MarketImpactEstimator::estimateSlippage(opp.longBook, opp.capital);
    double slippageSell = MarketImpactEstimator::estimateSlippage(opp.shortBook, opp.capital);

    double adjustedBuyPrice = entryBuy * (1 + slippageBuy / 100.0);
    double adjustedSellPrice = entrySell * (1 - slippageSell / 100.0);

    double profit = (adjustedSellPrice - adjustedBuyPrice) * quantity;


    ExecutedTrade trade{
        .symbol = opp.symbol,
        .buyExchange = opp.longExchange,
        .sellExchange = opp.shortExchange,
        .buyPrice = entryBuy,
        .sellPrice = entrySell,
        .capitalUsed = capitalUsed,
        .profit = profit,
        .timestamp = std::chrono::system_clock::now()
    };

    tradeHistory.push_back(trade);
    totalProfit += profit;
    VaREstimator::addPnL(profit);

    std::cout << "\n✅ Executed Trade:\n";
    std::cout << "🔹 Symbol: " << opp.symbol << "\n";
    std::cout << "🟢 Buy: " << trade.buyExchange << " at " << entryBuy << "\n";
    std::cout << "🔴 Sell: " << trade.sellExchange << " at " << entrySell << "\n";
        std::cout << "💰 Capital Used: " << capitalUsed << "\n";
    std::cout << "📉 Adjusted Buy Price (with slippage): " << adjustedBuyPrice << "\n";
    std::cout << "📈 Adjusted Sell Price (with slippage): " << adjustedSellPrice << "\n";

    std::cout << "📈 Profit: " << std::fixed << std::setprecision(2) << profit << " USDT\n";
}



double TradeExecutor::getTotalProfit() {
    return totalProfit;
}

int TradeExecutor::getTradeCount() {
    return static_cast<int>(tradeHistory.size());
}

const std::vector<ExecutedTrade>& TradeExecutor::getTradeHistory() {
    return tradeHistory;
}

void TradeExecutor::printPnLSummary() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "Timestamp: " << std::put_time(std::localtime(&now), "%F %T") << "\n";
    std::cout << "\n📊 === P&L SUMMARY ===\n";
    std::cout << "Total Trades Executed: " << getTradeCount() << "\n";
    std::cout << "Total Profit: " << std::fixed << std::setprecision(2) << totalProfit << " USDT\n";
}

void TradeExecutor::writeTradeHistoryToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "❌ Failed to write trade history to CSV: " << filename << "\n";
        return;
    }

    file << "Timestamp,Symbol,BuyExchange,SellExchange,BuyPrice,SellPrice,CapitalUsed,Profit\n";

    for (const auto& trade : tradeHistory) {
        std::time_t ts = std::chrono::system_clock::to_time_t(trade.timestamp);
        file << std::put_time(std::localtime(&ts), "%F %T") << ",";
        file << trade.symbol << "," << trade.buyExchange << "," << trade.sellExchange << ",";
        file << trade.buyPrice << "," << trade.sellPrice << ",";
        file << trade.capitalUsed << "," << trade.profit << "\n";
    }

    file.close();
    std::cout << "📝 Trade history saved to: " << filename << "\n";
}

