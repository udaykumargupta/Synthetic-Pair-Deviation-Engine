#include "arbitrage/StatisticalArbitrageEngine.hpp"
#include <numeric>
#include <cmath>
#include <unordered_map>

std::unordered_map<std::string, std::deque<double>> StatisticalArbitrageEngine::spreadHistoryMap;

void StatisticalArbitrageEngine::updateSpreadHistory(const std::string& key, double spread) {
    auto& history = spreadHistoryMap[key];
    history.push_back(spread);
    if (history.size() > MAX_HISTORY) history.pop_front();
}

double StatisticalArbitrageEngine::computeZScore(const std::string& key, double currentSpread) {
    const auto& history = spreadHistoryMap[key];
    if (history.size() < 20) return 0.0; // Not enough data

    double mean = std::accumulate(history.begin(), history.end(), 0.0) / history.size();

    double sq_sum = 0.0;
    for (double val : history) sq_sum += (val - mean) * (val - mean);
    double stddev = std::sqrt(sq_sum / history.size());

    return stddev == 0.0 ? 0.0 : (currentSpread - mean) / stddev;
}

bool StatisticalArbitrageEngine::isMeanReversionSignal(const std::string& key, double currentSpread, double thresholdZScore) {
    updateSpreadHistory(key, currentSpread);
    double z = computeZScore(key, currentSpread);
    return std::abs(z) >= thresholdZScore;
}
