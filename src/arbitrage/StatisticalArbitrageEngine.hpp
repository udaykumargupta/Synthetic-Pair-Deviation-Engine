#pragma once
#include <deque>
#include <string>
#include<iostream>
#include <unordered_map>

class StatisticalArbitrageEngine {
public:
    static void updateSpreadHistory(const std::string& key, double spread);
    static bool isMeanReversionSignal(const std::string& key, double currentSpread, double thresholdZScore);
    static double computeZScore(const std::string& key, double currentSpread);

private:
    static std::unordered_map<std::string, std::deque<double>> spreadHistoryMap;
    static const size_t MAX_HISTORY = 100;
};
