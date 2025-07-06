cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0167 NEW)

# Project and C++20
project(SyntheticPairDeviationEngine VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#  Vcpkg toolchain hint (if you're not passing it via CLI)
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

#  Dependencies
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)
# find_package(asio REQUIRED)       # Uncomment if using standalone ASIO via vcpkg
# find_package(websocketpp REQUIRED)  # Only if you installed websocketpp via vcpkg

# ✅ Sources (remove duplicate)
set(SOURCE_FILES
    src/main.cpp
    src/arbitrage/SyntheticInstrumentCalculator.cpp
    src/exchange/BinanceClient.cpp
    src/exchange/OKXClient.cpp
    src/exchange/BybitClient.cpp
    src/utils/Logger.cpp
    src/exchange/MarketDataStore.cpp 
    src/exchange/MarketDataAggregator.cpp
    src/arbitrage/RiskManager.cpp
    src/arbitrage/TradeExecutor.cpp 
    src/exchange/BinancePerpClient.cpp
    src/monitoring/PerformanceMonitor.cpp
    src/arbitrage/LiquidityAnalyzer.cpp
    src/arbitrage/options/OptionPricer.cpp
    src/arbitrage/VolatilityArbitrage.cpp
    src/arbitrage/StatisticalArbitrageEngine.cpp
    src/monitoring/RiskDashboard.cpp
    src/monitoring/VaREstimator.cpp
    src/monitoring/StressTester.cpp
)

add_executable(arb_engine ${SOURCE_FILES})

# ✅ Includes
target_include_directories(arb_engine PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/external/websocketpp
)

# ✅ Linking
target_link_libraries(arb_engine
    PRIVATE
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
    ws2_32  # Windows socket library
)

# ✅ Compile definitions
target_compile_definitions(arb_engine PRIVATE
    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_STL_
    BOOST_ASIO_NO_DEPRECATED
)

# ✅ Optional testing
include(CTest)
enable_testing()
