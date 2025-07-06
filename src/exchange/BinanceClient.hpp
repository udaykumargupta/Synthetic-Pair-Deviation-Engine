#pragma once
#include "ExchangeClient.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>

using ws_client = websocketpp::client<websocketpp::config::asio_tls_client>;

class BinanceClient : public ExchangeClient {
public:
    void handleIncomingMessage(const std::string& msg);
    BinanceClient(const std::string& symbol);
    void connect() override;
    void disconnect() override;
    std::string name() const override { return "Binance"; }

private:
    std::string symbol;
    bool connected = false;
    ws_client client;
    std::thread runThread;
    websocketpp::connection_hdl conn_hdl; // ✅ Moved here
};
