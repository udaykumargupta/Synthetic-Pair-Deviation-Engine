#include "exchange/BinanceClient.hpp"
#include "exchange/OKXClient.hpp"
#include "exchange/BybitClient.hpp"
#include "exchange/MarketDataTypes.hpp"
#include "exchange/MarketDataAggregator.hpp"
#include "exchange/BinancePerpClient.hpp"
#include "arbitrage/SyntheticInstrumentCalculator.hpp"
#include "arbitrage/ArbitrageOpportunity.hpp"
#include "arbitrage/ArbitrageLegOptimizer.hpp"
#include "arbitrage/RiskManager.hpp"
#include "arbitrage/TradeExecutor.hpp"
#include "monitoring/PerformanceMonitor.hpp"
#include "arbitrage/options/VolatilityArbitrageDetector.hpp"
#include "arbitrage/VolatilityArbitrage.hpp"
#include "arbitrage/StatisticalArbitrageEngine.hpp"
#include "monitoring/RiskDashboard.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <windows.h>
#include "monitoring/VaREstimator.hpp"
#include "monitoring/StressTester.hpp"
#include "arbitrage/risk/CorrelationAnalyzer.hpp"

void checkSyntheticFutures(MarketDataAggregator &aggregator)
{
    const auto &latestUpdates = aggregator.getLatestUpdates();
    if (!latestUpdates.count("Binance") || !latestUpdates.count("OKX"))
        return;

    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "🔍 SYNTHETIC FUTURES ANALYSIS\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

    const auto &binancePerp = latestUpdates.at("Binance");
    const auto &okxSpot = latestUpdates.at("OKX");

    double realSpot = (okxSpot.bestBid + okxSpot.bestAsk) / 2.0;
    SyntheticInstrument syntheticSpot = SyntheticInstrumentCalculator::computeSyntheticSpot(binancePerp, 0.0005, 2.0);

    auto fundingDataOpt = aggregator.getFundingData("Binance");
    if (!fundingDataOpt)
        return;

    double fundingRate = fundingDataOpt->fundingRate;
    SyntheticInstrument syntheticFuture = SyntheticInstrumentCalculator::computeSyntheticFuture_FundingModel(okxSpot, fundingRate, 7.0 / 365.0);

    double mispricing1 = SyntheticInstrumentCalculator::computeMispricing(realSpot, syntheticSpot.price);
    double mispricing2 = SyntheticInstrumentCalculator::computeMispricing(realSpot, syntheticFuture.price);

    std::cout << "📊 Real Spot (OKX): " << realSpot << "\n";
    std::cout << "🧮 Synthetic Spot (Binance): " << syntheticSpot.price << " → Mispricing: " << mispricing1 << "%\n";
    std::cout << "🧮 Synthetic Future (Funding Model): " << syntheticFuture.price << " → Mispricing: " << mispricing2 << "%\n";

    RiskDashboard::displayFundingImpact("BTC/USDT", fundingRate, 10000.0);
    RiskDashboard::displayLiquidityAlert("BTC/USDT", okxSpot, 2.0);
    RiskDashboard::displayBasisRisk("BTC/USDT", realSpot, syntheticFuture.price);

    double spread = syntheticSpot.price - realSpot;
    StatisticalArbitrageEngine::updateSpreadHistory("BTC_SPOT_SYNTH", spread);
    if (StatisticalArbitrageEngine::isMeanReversionSignal("BTC_SPOT_SYNTH", spread, 2.0))
    {
        std::cout << "📈 Stat-Arb Signal: Spread deviation detected (Z-Score ≥ 2)\n";
    }

    double capital1 = ArbitrageLegOptimizer::computeCapitalLimit(okxSpot, binancePerp, 10000.0);
    double capital2 = ArbitrageLegOptimizer::computeCapitalLimit(okxSpot, okxSpot, 10000.0);

    ArbitrageOpportunity arb1 = SyntheticInstrumentCalculator::evaluateArbitrage("BTC/USDT", "OKX", "Binance", realSpot, syntheticSpot.price, 0.1, capital1, okxSpot, binancePerp);
    ArbitrageOpportunity arb2 = SyntheticInstrumentCalculator::evaluateArbitrage("BTC/USDT", "OKX", "OKX", realSpot, syntheticFuture.price, 0.1, capital2, okxSpot, binancePerp);

    if (!arb1.longExchange.empty() && RiskManager::isRiskAcceptable(arb1, okxSpot)) {
        arb1.strategyType = "Spot vs Synthetic Spot";
        std::cout << arb1.describe();
        TradeExecutor::executeTrade(arb1);
    }

    if (!arb2.longExchange.empty() && RiskManager::isRiskAcceptable(arb2, okxSpot)) {
        arb2.strategyType = "Spot vs Synthetic Future";
        std::cout << arb2.describe();
        TradeExecutor::executeTrade(arb2);
    }

    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}

void checkCrossExchangeSpotArb(MarketDataAggregator &aggregator)
{
    const auto &latestUpdates = aggregator.getLatestUpdates();
    if (!latestUpdates.count("Binance") || !latestUpdates.count("Bybit"))
        return;

    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "🔍 CROSS-EXCHANGE SPOT ARBITRAGE\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

    const auto &binanceReal = latestUpdates.at("Binance");
    const auto &bybitReal = latestUpdates.at("Bybit");

    double binanceMid = (binanceReal.bestBid + binanceReal.bestAsk) / 2.0;
    double bybitMid = (bybitReal.bestBid + bybitReal.bestAsk) / 2.0;

    CorrelationAnalyzer::updatePrice("BTC_BINANCE", binanceMid);
    CorrelationAnalyzer::updatePrice("BTC_BYBIT", bybitMid);
    CorrelationAnalyzer::displayAlertIfDiverging("BTC_BINANCE", "BTC_BYBIT");

    double mispricing = SyntheticInstrumentCalculator::computeMispricing(bybitMid, binanceMid);
    std::cout << "≡ Cross-Exchange Mispricing (Binance vs Bybit): " << mispricing << "%\n";

    double capital = ArbitrageLegOptimizer::computeCapitalLimit(bybitReal, binanceReal, 10000.0);
    ArbitrageOpportunity arb = SyntheticInstrumentCalculator::evaluateArbitrage("BTC/USDT", "Bybit", "Binance", bybitMid, binanceMid, 0.1, capital, bybitReal, binanceReal);

    if (!arb.longExchange.empty() && RiskManager::isRiskAcceptable(arb, bybitReal)) {
        arb.strategyType = "Cross-Exchange Spot Arbitrage";
        std::cout << arb.describe();
        TradeExecutor::executeTrade(arb);
    }

    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}


void checkSyntheticVsRealSpot(MarketDataAggregator &aggregator)
{
    const auto &latestUpdates = aggregator.getLatestUpdates();
    if (!latestUpdates.count("Binance") || !latestUpdates.count("Bybit"))
        return;

    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "🔍 SYNTHETIC VS REAL SPOT (BINANCE vs BYBIT)\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

    const auto &binancePerp = latestUpdates.at("Binance");
    const auto &bybitSpot = latestUpdates.at("Bybit");

    RiskDashboard::displayLiquidityAlert("BTC/USDT", bybitSpot, 2.0);
    RiskDashboard::displayLiquidityAlert("BTC/USDT", binancePerp, 2.0);

    SyntheticInstrument binanceSynthetic = SyntheticInstrumentCalculator::computeSyntheticSpot(binancePerp, 0.0005, 2.0);
    double realBybit = (bybitSpot.bestBid + bybitSpot.bestAsk) / 2.0;

    double mispricing = SyntheticInstrumentCalculator::computeMispricing(realBybit, binanceSynthetic.price);
    std::cout << "≡ Mispricing (Synthetic Spot vs Real Spot): " << mispricing << "%\n";

    double capital = ArbitrageLegOptimizer::computeCapitalLimit(bybitSpot, binancePerp, 10000.0);
    ArbitrageOpportunity arb = SyntheticInstrumentCalculator::evaluateArbitrage("BTC/USDT", "Bybit", "Binance", realBybit, binanceSynthetic.price, 0.1, capital, bybitSpot, binancePerp);

    if (!arb.longExchange.empty() && RiskManager::isRiskAcceptable(arb, bybitSpot)) {
        arb.strategyType = "Synthetic Spot vs Real Spot";
        std::cout << arb.describe();
        TradeExecutor::executeTrade(arb);
    }

    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}

void runStressTest(MarketDataAggregator &aggregator)
{
    const auto &latestUpdates = aggregator.getLatestUpdates();
    if (!latestUpdates.count("Binance"))
        return;

    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "🧪 STRESS TEST - PRICE SHOCK SIMULATION\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

    const auto &binance = latestUpdates.at("Binance");
    OrderBookUpdate shockedBook = StressTester::simulatePriceShock(binance, -20.0);

    std::cout << "⚠️ Simulated -20% Price Shock on Binance\n";
    std::cout << "Old Bid: " << binance.bestBid << " | New Bid: " << shockedBook.bestBid << "\n";

    SyntheticInstrument synthetic = SyntheticInstrumentCalculator::computeSyntheticSpot(shockedBook, 0.0005, 2.0);
    std::cout << "📉 Synthetic Spot Price After Shock: " << synthetic.price << "\n";

    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}


int main()
{
    SetConsoleOutputCP(CP_UTF8);

    MarketDataAggregator aggregator;
    std::vector<std::unique_ptr<ExchangeClient>> clients;

    clients.emplace_back(std::make_unique<BinanceClient>("btcusdt"));
    clients.emplace_back(std::make_unique<OKXClient>("BTC-USDT"));
    clients.emplace_back(std::make_unique<BybitClient>("BTCUSDT"));

    auto binancePerp = std::make_unique<BinancePerpClient>("btcusdt");
    binancePerp->setMarkPriceCallback([&aggregator](double mark, double funding) {
        aggregator.updateFundingAndMark("Binance", mark, funding);
    });
    binancePerp->connect();

    for (auto &client : clients)
    {
        client->setOrderBookCallback([&aggregator, &client](const OrderBookUpdate &update) {
            aggregator.update(client->name(), update);
        });
        client->connect();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    int loopCount = 0;
    while (true)
    {
        PerformanceMonitor::startLatencyTimer();

        checkSyntheticFutures(aggregator);
        checkCrossExchangeSpotArb(aggregator);
        checkSyntheticVsRealSpot(aggregator);
        VolatilityArbitrage::checkVolatilityArbitrage(aggregator);

        std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "📸 MARKET SNAPSHOT\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        aggregator.printSnapshot();

        PerformanceMonitor::recordUpdate();
        PerformanceMonitor::stopLatencyTimer();

        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (++loopCount % 10 == 0)
        {
            runStressTest(aggregator);
            VaREstimator::printVaRReport();
            PerformanceMonitor::printMetrics();
            TradeExecutor::printPnLSummary();
            TradeExecutor::writeTradeHistoryToCSV("executed_trades.csv");
        }
    }

    return 0;
}

