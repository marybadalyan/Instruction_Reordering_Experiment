#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <vector>
#include "kaizen.h"  

// Repeat work to make timing measurable
const int ITERATIONS = 1'000'000;

int not_using_ROB() {
    volatile int a = 5;
    a *= 8;
    a %= 9;
    return a * 2;
}

int using_ROB() {
    volatile int a = 5;
    volatile int b = 3;
    a *= 8;
    b %= 9;
    return a + b;
}

// Single-threaded benchmark
long long benchmark_single_thread(int (*func)()) {
    zen::timer timer;
    timer.start();
    int result = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        result += func();
    }
    timer.stop();
    return timer.duration<zen::timer::nsec>().count();
}

// Multi-threaded benchmark (2 threads on Core 0)
long long benchmark_multi_thread(int (*func)()) {
    const int thread_count = 2;
    std::vector<std::thread> threads;
    volatile int results[thread_count] = {0}; // Store results per thread
    zen::timer timer;

    // Create threads with a barrier to delay execution
    std::vector<bool> ready(thread_count, false);
    DWORD_PTR affinity_mask = 1; // Core 0

    for (int t = 0; t < thread_count; t++) {
        threads.emplace_back([&results, &ready, t, func]() {
            // Spin until main signals ready (ensures affinity is set)
            while (!ready[t]) {
                std::this_thread::yield();
            }
            for (int i = 0; i < ITERATIONS; i++) {
                results[t] += func();
            }
        });

        // Set affinity immediately after creation, before execution
        HANDLE thread_handle = threads.back().native_handle();
        if (!SetThreadAffinityMask(thread_handle, affinity_mask)) {
            std::cerr << "Failed to set affinity for thread " << t << "\n";
        }
    }

    // Start timing and signal threads to begin
    timer.start();
    for (int t = 0; t < thread_count; t++) {
        ready[t] = true; // Release threads
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }
    timer.stop();

    int total_result = 0;
    for (int t = 0; t < thread_count; t++) {
        total_result += results[t];
    }

    return timer.duration<zen::timer::nsec>().count();
}

int main() {
    // Warm-up to stabilize CPU frequency
    for (int i = 0; i < 1000; i++) {
        using_ROB();
        not_using_ROB();
    }

    // Single-threaded benchmarks
    auto single_not_using = benchmark_single_thread(not_using_ROB);
    auto single_using = benchmark_single_thread(using_ROB);

    // Multi-threaded benchmarks
    auto multi_not_using = benchmark_multi_thread(not_using_ROB);
    auto multi_using = benchmark_multi_thread(using_ROB);

    zen::print("Single-threaded not_using_ROB: ", single_not_using,'\n');
    zen::print("Single-threaded using_ROB: ",single_using,'\n');
    zen::print("Multi-threaded not_using_ROB: ",multi_not_using,'\n');
    zen::print("Multi-threaded using_ROB: ",multi_using,'\n');

    return 0;
}