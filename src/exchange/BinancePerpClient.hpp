#pragma once

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <nlohmann/json.hpp>
#include <asio/ssl.hpp>
#include <functional>
#include <string>
#include <thread>

using MarkPriceCallback = std::function<void(double markPrice, double fundingRate)>;

class BinancePerpClient {
public:
    BinancePerpClient(const std::string& symbol);
    void connect();
    void disconnect();

    void setMarkPriceCallback(MarkPriceCallback cb) { markPriceCallback = std::move(cb); }
    std::string name() const { return "BinancePerp"; }

private:
    void handleIncomingMessage(const std::string& message);

    std::string symbol;
    std::string lowerSymbol;
    websocketpp::client<websocketpp::config::asio_tls_client> ws;
    websocketpp::connection_hdl conn_hdl;
    bool connected;
    std::thread wsThread;

    MarkPriceCallback markPriceCallback;
};
