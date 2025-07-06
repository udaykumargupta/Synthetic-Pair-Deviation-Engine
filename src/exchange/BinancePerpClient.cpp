#include "exchange/BinancePerpClient.hpp"
#include <iostream>
#include <filesystem>
#include <iomanip>

using websocketpp::lib::error_code;
using json = nlohmann::json;

BinancePerpClient::BinancePerpClient(const std::string& symbol)
    : symbol(symbol), connected(false) {
    lowerSymbol = symbol;
    std::transform(lowerSymbol.begin(), lowerSymbol.end(), lowerSymbol.begin(), ::tolower);

    ws.clear_access_channels(websocketpp::log::alevel::all);
    ws.init_asio();

    ws.set_tls_init_handler([](websocketpp::connection_hdl) {
        return std::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);
    });

    ws.set_message_handler([this](websocketpp::connection_hdl, auto msg) {
        handleIncomingMessage(msg->get_payload());
    });
}

void BinancePerpClient::connect() {
    std::string uri = "wss://fstream.binance.com/ws/!markPrice@arr";

    error_code ec;
    auto con = ws.get_connection(uri, ec);
    if (ec) {
        std::cerr << "❌ BinancePerp connection error: " << ec.message() << "\n";
        return;
    }

    conn_hdl = con->get_handle();
    ws.connect(con);

    connected = true;

    wsThread = std::thread([this]() {
        ws.run();
    });

    std::cout << "🔌 Connected to Binance perpetual\n";
}


void BinancePerpClient::disconnect() {
    if (connected) {
        error_code ec;
        ws.close(conn_hdl, websocketpp::close::status::normal, "Disconnect", ec);
        if (wsThread.joinable()) {
            wsThread.join();
        }
        connected = false;
    }
}

void BinancePerpClient::handleIncomingMessage(const std::string& payload) {
    try {
        auto j = nlohmann::json::parse(payload);

        if (!j.is_array()) {
            std::cerr << "❌ Expected JSON array but got object. Message: " << payload << std::endl;
            return;
        }

        for (const auto& entry : j) {
            if (entry.contains("s") && entry["s"] == "BTCUSDT") {
                double mark = std::stod(entry["p"].get<std::string>());
                double funding = std::stod(entry["r"].get<std::string>());

                std::cout << std::fixed << std::setprecision(10);
                std::cout << "[DEBUG] Parsed fundingRate: " << funding << ", markPrice: " << mark << std::endl;

                if (markPriceCallback) {
                    markPriceCallback(mark, funding);
                }
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ BinancePerpClient parse error: " << e.what() << std::endl;
    }
}



