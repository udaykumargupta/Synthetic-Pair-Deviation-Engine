#pragma once
#include <string>
#include <vector>
#include <chrono>

using Timestamp = std::chrono::system_clock::time_point;

struct OrderBookUpdate {
    std::string symbol;
    double bestBid;
    double bestAsk;
    std::vector<std::pair<double, double>> bids; // price, quantity
    std::vector<std::pair<double, double>> asks;
    Timestamp timestamp;
    double bestBidQty;
    double bestAskQty;
    double bidQty = 0.0;
    double askQty = 0.0;
};
