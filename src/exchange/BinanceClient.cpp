#include "BinanceClient.hpp"
#include "MarketDataTypes.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>

using json = nlohmann::json;
using context_ptr = std::shared_ptr<asio::ssl::context>;
using ws_connection_hdl = websocketpp::connection_hdl;

// TLS Init Handler
context_ptr on_tls_init(websocketpp::connection_hdl) {
    auto ctx = std::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);

    try {
        ctx->set_options(
            asio::ssl::context::default_workarounds |
            asio::ssl::context::no_sslv2 |
            asio::ssl::context::no_sslv3 |
            asio::ssl::context::single_dh_use
        );

        // ❌ WARNING: This disables verification
        ctx->set_verify_mode(asio::ssl::verify_none);
        // std::cout << "⚠️ TLS verification DISABLED. Not secure!\n";

    } catch (const std::exception& e) {
        std::cerr << "TLS Init Error: " << e.what() << std::endl;
    }

    return ctx;
}



BinanceClient::BinanceClient(const std::string& symbol)
    : symbol(symbol), connected(false) {}

void BinanceClient::connect() {
    client.set_access_channels(websocketpp::log::alevel::all);
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    client.set_error_channels(websocketpp::log::elevel::all);

    client.init_asio();
    client.start_perpetual();  // 🔄 Keeps io_service running

    client.set_tls_init_handler([](websocketpp::connection_hdl hdl) {
        return on_tls_init(hdl);
    });

    std::string url = "wss://stream.binance.com:443/stream?streams=" + symbol + "@bookTicker";
    // std::string url="wss://stream.binance.com:443/stream?streams=btcusdt@bookTicker";

    // std::cout << "GET URL: " << url << std::endl;


    client.set_message_handler([this](ws_connection_hdl, ws_client::message_ptr msg) {
        handleIncomingMessage(msg->get_payload());
    });

    websocketpp::lib::error_code ec;
    auto con = client.get_connection(url, ec);

    if (ec) {
        std::cout << "❌ Connection error: " << ec.message() << std::endl;
        return;
    }

    conn_hdl = con->get_handle(); // ✅ Save connection handle
    client.connect(con);

    runThread = std::thread([this]() {
        client.run();
    });

    connected = true;
    std::cout << "🔌 Connected to Binance spot " << "\n";
}

void BinanceClient::disconnect() {
    if (connected) {
        websocketpp::lib::error_code ec;

        client.close(conn_hdl, websocketpp::close::status::going_away, "Client disconnect", ec);
        if (ec) {
            std::cerr << "❌ Close error: " << ec.message() << std::endl;
        }

        client.stop_perpetual();

        if (runThread.joinable()) {
            runThread.join();
        }

        connected = false;
        std::cout << "🔌 Disconnected from Binance.\n";
    }
}

void BinanceClient::handleIncomingMessage(const std::string& msg) {
    try {
        auto j = json::parse(msg);
        auto data = j["data"];

        OrderBookUpdate update;
        update.symbol = data.value("s", "");
        update.bestBid = std::stod(data.value("b", "0.0"));
        update.bestAsk = std::stod(data.value("a", "0.0"));
        update.bestBidQty = std::stod(data.value("B", "0.0"));  
        update.bestAskQty = std::stod(data.value("A", "0.0")); 

        if (orderBookCallback) {
            orderBookCallback(update);
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Error parsing message: " << e.what() << std::endl;
    }
}