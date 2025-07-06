#pragma once
#include <string>
#include <functional>

struct OrderBookUpdate;  // Forward declare or include from MarketDataTypes

class ExchangeClient {
public:
    virtual ~ExchangeClient() = default;

    // Start listening to market data
    virtual void connect() = 0;

    // Close WebSocket connection
    virtual void disconnect() = 0;

    // Return name of the exchange
    virtual std::string name() const = 0;

    // Optional: handler setter
    void setOrderBookCallback(std::function<void(const OrderBookUpdate&)> cb) {
        orderBookCallback = cb;
    }

protected:
    std::function<void(const OrderBookUpdate&)> orderBookCallback;
};
