# Synthetic-Pair-Deviation-Engine

A high-performance, modular C++ engine that detects **real-time arbitrage opportunities** by analyzing **price deviations between real and synthetic derivatives** across top cryptocurrency exchanges like **Binance**, **Bybit**, and **OKX**.

It supports arbitrage detection in spot, perpetual swaps, futures, and optionally options, using robust financial models and real-time WebSocket feeds.

---

##  Objective

To build a backend engine that:
- Analyzes real-time market data from multiple exchanges
- Constructs synthetic instruments (synthetic spot/derivatives)
- Detects arbitrage opportunities and mispricings
- Apply risk filters for liquidity, slippage, volatility, and position size
- Recommends optimal arbitrage legs with risk-managed exposure
- Monitor performance metrics, PnL, and execution logs
- Issue alerts based on funding impact, price divergence, or abnormal correlation

---

##  Features Implemented

###  Core Engine
- Real-time market data processing via WebSockets
- Cross-exchange synthetic instrument construction
- Detection of bidirectional arbitrage opportunities
- Pricing model for synthetic perpetuals and futures
- Mispricing and profit threshold detection algorithm

###  Synthetic Derivatives Support
- Spot + funding rate → Synthetic perpetual
- Spot + cost of carry → Synthetic future
- Cross-synthetic arbitrage detection

###  Real-Time Monitoring
- Performance metrics (latency, throughput)
- P&L summary and trade history CSV export
- Risk alerts for slippage and funding rate changes
- Statistical arbitrage with Z-score deviation monitoring

###  Modular Architecture
- Multi-threaded data ingestion
- Thread-safe market data aggregator
- Configurable profit thresholds and position sizing
- Position sizing and capital allocation optimization

###  WebSocket Engine
- Connects to:
    -Binance (spot & perp)
    -Bybit (spot)
    -OKX (spot)
- Streams:
    -Level-2 orderbook updates
    -Mark price, index price, funding rate
    -Trade tickers and spreads
-Handles different data formats, protocols, and heartbeat mechanisms
-Maintains synchronized state for multi-exchange data
  
---

##  Technology Stack

- C++20 with standard library concurrency
- Build System: CMake ≥ 3.15
- Networking: WebSocket++ + ASIO
- JSON Parsing: nlohmann/json
- Modern memory management (RAII, smart pointers)
- Multithreading: Thread-safe MarketDataAggregator, mutex-free read access
- Data Structures: Optimized for real-time processing and low-latency computation

---

## 🛠️ Build Instructions

### Dependencies
Install via vcpkg or your system's package manager:
  -asio
  -websocketpp
  -nlohmann-json
  -openssl (for secure WebSocket APIs)
  -CMake (≥ 3.15)
  -GCC / MSVC / Clang with C++20 support

---
## Input Parameters

| Parameter                   | Description                                          |
| --------------------------- | ---------------------------------------------------- |
| Exchange Selection          | Binance, Bybit, OKX (configurable)                   |
| Asset Selection             | `BTC/USDT`, `ETH/USDT`, etc.                         |
| Derivative Types            | Spot, Perpetuals, Futures (Options - optional)       |
| Minimum Profit Threshold    | % profit required to trigger trade suggestion        |
| Maximum Position Size       | Capital limit per opportunity (risk control)         |
| Synthetic Construction Args | Funding rate, leverage, expiry window, cost-of-carry |

---
##  Output Parameters
| Output                    | Description                                                        |
| ------------------------- | ------------------------------------------------------------------ |
| Mispricing Opportunities  | Real-time detected % spread between real vs synthetic price        |
| Arbitrage Leg Suggestions | Long/short exchanges, expected returns, and required capital       |
| Synthetic Pricing Reports | Pricing breakdown of synthetic futures/perpetuals                  |
| Risk Metrics              | Funding impact, basis risk, correlation warnings, liquidity alerts |
| Performance Metrics       | Latency, throughput, memory/cpu usage                              |
| Execution Logs & PnL      | Trades executed, profit/loss summary, CSV trade history export     |

---
## Performance & Monitoring
| Metric                  | Description                                |
| ----------------------- | ------------------------------------------ |
| ⏱️ Latency              | ≤ 10 ms from data arrival to decision      |
| ⚙️ Throughput           | Handles >2000 updates/sec                  |
| 🧠 Memory & CPU Usage   | Efficient threading and smart allocations  |
| 🧾 PnL Summary          | Trade results + history in CSV             |
| 📊 VaR + Stress Testing | Simulated shocks and downside risk reports |

---
## Clone the Repository
```bash
git clone https://github.com/udaykumargupta/Synthetic-Pair-Deviation-Engine.git
cd Synthetic-Pair-Deviation-Engine
```
---

## Author
**Uday Kumar Gupta**
B.Tech CSE @ IIIT Kota
📧 udaykumargupta1104@gmail.com
