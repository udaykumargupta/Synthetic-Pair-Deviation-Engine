#include "BybitClient.hpp"
#include "MarketDataTypes.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>

using json = nlohmann::json;
using context_ptr = std::shared_ptr<asio::ssl::context>;

BybitClient::BybitClient(const std::string& symbol) : symbol(symbol), connected(false) {
    ws.clear_access_channels(websocketpp::log::alevel::all);
    ws.init_asio();

    ws.set_tls_init_handler([](websocketpp::connection_hdl) -> context_ptr {
        return std::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);
    });

    ws.set_message_handler([this](websocketpp::connection_hdl, WebSocketClient::message_ptr msg) {
        handleIncomingMessage(msg->get_payload());
    });
}

void BybitClient::connect() {
    std::string uri = "wss://stream.bybit.com/v5/public/linear";
    websocketpp::lib::error_code ec;
    auto con = ws.get_connection(uri, ec);
    if (ec) {
        std::cerr << "Bybit connection error: " << ec.message() << std::endl;
        return;
    }

    con->set_open_handler([con](websocketpp::connection_hdl) {
        std::string subscription = R"({
            "op": "subscribe",
            "args": ["orderbook.1.BTCUSDT"]
        })";
        con->send(subscription);
        std::cout << "🔌 Connected to Bybit spot"<<"\n";
    });

    ws.connect(con);
    std::thread([this]() {
        ws.run();
    }).detach();
}


void BybitClient::disconnect() {
    ws.stop();
}

std::string BybitClient::name() const {
    return "Bybit";
}

void BybitClient::handleIncomingMessage(const std::string& msg) {
    // std::cout << "[Bybit Raw] " << msg << std::endl;

    try {
        auto j = json::parse(msg);
        if (j.contains("data") && j["data"].contains("b") && j["data"].contains("a")) {
            const auto& bids = j["data"]["b"];
            const auto& asks = j["data"]["a"];

            if (!bids.empty() && !asks.empty()) {
                OrderBookUpdate update;
                update.symbol = "BTCUSDT";

                // ✅ Prices
                update.bestBid = std::stod(bids[0][0].get<std::string>());
                update.bestAsk = std::stod(asks[0][0].get<std::string>());

                // ✅ Quantities
                update.bestBidQty = std::stod(bids[0][1].get<std::string>());
                update.bestAskQty = std::stod(asks[0][1].get<std::string>());

                update.timestamp = std::chrono::system_clock::now();

                if (orderBookCallback) {
                    orderBookCallback(update);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Bybit parse error: " << e.what() << std::endl;
    }
}


