#include "monitoring/StressTester.hpp"

OrderBookUpdate StressTester::simulatePriceShock(const OrderBookUpdate& original, double shockPercent) {
    double factor = 1.0 + (shockPercent / 100.0);  // e.g., -20% = 0.8
    OrderBookUpdate shocked = original;
    shocked.bestBid *= factor;
    shocked.bestAsk *= factor;
    return shocked;
}
