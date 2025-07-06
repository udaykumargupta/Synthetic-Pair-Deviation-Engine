#include "OKXClient.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <filesystem> // ✅ Needed for checking cert path
#include "MarketDataTypes.hpp"

using json = nlohmann::json;
using WebSocketClient = websocketpp::client<websocketpp::config::asio_tls_client>;

OKXClient::OKXClient(const std::string& symbol) : symbol(symbol), connected(false) {
    ws.clear_access_channels(websocketpp::log::alevel::all);  // Optional: disable logs
    ws.init_asio();

    // ✅ TLS Handler
    ws.set_tls_init_handler([](websocketpp::connection_hdl) {
        auto ctx = std::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);
        try {
            ctx->set_options(
                asio::ssl::context::default_workarounds |
                asio::ssl::context::no_sslv2 |
                asio::ssl::context::no_sslv3 |
                asio::ssl::context::no_tlsv1 |
                asio::ssl::context::no_tlsv1_1
            );

            std::string certPath = R"(D:\SyntheticPairDeviationEngine\certs\cacert.pem)";
            if (!std::filesystem::exists(certPath)) {
                std::cerr << "❌ CA file not found at: " << certPath << std::endl;
            }

            ctx->set_verify_mode(asio::ssl::verify_peer);
            ctx->load_verify_file(certPath);
        } catch (const std::exception& e) {
            std::cerr << "TLS Init Error: " << e.what() << std::endl;
        }

        return ctx;
    });

    // ✅ Message Handler
    ws.set_message_handler([this](websocketpp::connection_hdl, WebSocketClient::message_ptr msg) {
        handleIncomingMessage(msg->get_payload());
    });
}

void OKXClient::connect() {
    const std::string uri = "wss://ws.okx.com:443/ws/v5/public";
    websocketpp::lib::error_code ec;
    auto con = ws.get_connection(uri, ec);

    if (ec) {
        std::cerr << "❌ OKX connection error: " << ec.message() << std::endl;
        return;
    }

    // std::cout << "🟡 Connecting to OKX WebSocket...\n";

    // ✅ Open Handler to send subscription
    con->set_open_handler([this](websocketpp::connection_hdl hdl) {
    const std::string subscription = R"({
        "op": "subscribe",
        "args": [{
            "channel": "books5",
            "instId": "BTC-USDT"
        }]
    })";

    websocketpp::lib::error_code ec;

    
    ws.send(hdl, subscription, websocketpp::frame::opcode::text, ec);

    if (ec) {
        std::cerr << "❌ OKX subscription error: " << ec.message() << std::endl;
    } else {
        std::cout << "🔌 Connected to OKX spot\n";
        connected = true;
    }
});

    ws.connect(con);

    std::thread([this]() {
        ws.run();
    }).detach();
}

void OKXClient::disconnect() {
    ws.stop();
}

std::string OKXClient::name() const {
    return "OKX";
}

void OKXClient::handleIncomingMessage(const std::string& payload) {
    try {
        auto j = json::parse(payload);

        if (!j.contains("data") || j["data"].empty()) return;
        const auto& data = j["data"][0];

        if (!data.contains("bids") || !data.contains("asks")) return;
        const auto& bids = data["bids"];
        const auto& asks = data["asks"];

        if (bids.empty() || asks.empty()) return;

        OrderBookUpdate update;
        update.symbol = symbol;

        // ✅ Parse prices and quantities
        update.bestBid = std::stod(bids[0][0].get<std::string>());
        update.bestAsk = std::stod(asks[0][0].get<std::string>());
        update.bestBidQty = std::stod(bids[0][1].get<std::string>());
        update.bestAskQty = std::stod(asks[0][1].get<std::string>());
        update.timestamp = std::chrono::system_clock::now();

        if (orderBookCallback) {
            orderBookCallback(update);
        }

    } catch (const std::exception& e) {
        std::cerr << "❌ OKX parse error: " << e.what() << std::endl;
    }
}
