#pragma once
#include "exchange/MarketDataTypes.hpp"

class StressTester {
public:
    static OrderBookUpdate simulatePriceShock(const OrderBookUpdate& original, double shockPercent);
};
