#include "PerformanceMonitor.hpp"
#include <iostream>
#include <iomanip>

std::chrono::steady_clock::time_point PerformanceMonitor::startTime;
std::chrono::duration<double> PerformanceMonitor::totalLatency{};
std::atomic<int> PerformanceMonitor::updateCount{0};

void PerformanceMonitor::startLatencyTimer() {
    startTime = std::chrono::steady_clock::now();
}

void PerformanceMonitor::stopLatencyTimer() {
    auto endTime = std::chrono::steady_clock::now();
    totalLatency += endTime - startTime;
}

void PerformanceMonitor::recordUpdate() {
    ++updateCount;
}

void PerformanceMonitor::printMetrics() {
    double avgLatencyMs = (updateCount > 0)
        ? (totalLatency.count() / updateCount) * 1000.0
        : 0.0;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "📊 Performance Metrics:\n";
    std::cout << "   ➤ Market Updates Processed: " << updateCount << "\n";
    std::cout << "   ➤ Avg Detection Latency: " << avgLatencyMs << " ms\n";

    // Reset for next window
    totalLatency = std::chrono::duration<double>::zero();
    updateCount = 0;
}
