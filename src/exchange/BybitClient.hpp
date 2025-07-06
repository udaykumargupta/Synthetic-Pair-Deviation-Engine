#pragma once
#include "ExchangeClient.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <asio/ssl.hpp>

using WebSocketClient = websocketpp::client<websocketpp::config::asio_tls_client>;

class BybitClient : public ExchangeClient {
public:
    explicit BybitClient(const std::string& symbol);

    void connect() override;
    void disconnect() override;
    std::string name() const override;

private:
    std::string symbol;
    bool connected = false;

    WebSocketClient ws;

    void handleIncomingMessage(const std::string& msg);
};
