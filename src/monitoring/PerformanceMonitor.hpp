#pragma once
#include <chrono>
#include <atomic>

class PerformanceMonitor {
public:
    static void startLatencyTimer();
    static void stopLatencyTimer();
    static void recordUpdate();
    static void printMetrics();

private:
    static std::chrono::steady_clock::time_point startTime;
    static std::chrono::duration<double> totalLatency;
    static std::atomic<int> updateCount;
};
