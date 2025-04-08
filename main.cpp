#include <iostream>
#include <vector>
#include "kaizen.h"  
#include <format>
#include <stdio.h>

#include <atomic>
#include <memory>

// Parse command-line arguments
int process_args(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);
    auto iter_options = args.get_options("--iterations");

    if (iter_options.empty()) {
        zen::print("Error: --iterations  argument is absent, using default:iterations=1'000'0000\n");
        return {10000000}; // Increased defaults
    }
    return std::stoi(iter_options[0]);
}

int not_using_ROB(int x) {
    std::atomic<int> a(x);  // Initialize atomic with input x

    // Step 1: Multiply by 8
    int current = a.load(std::memory_order_seq_cst);
    int desired;
    do {
        desired = current * 8;
    } while (!a.compare_exchange_strong(current, desired, std::memory_order_seq_cst));

    // Step 2: Modulo 9
    current = a.load(std::memory_order_seq_cst);
    do {
        desired = current % 9;  // Simplified modulo
    } while (!a.compare_exchange_strong(current, desired, std::memory_order_seq_cst));

    // Step 3: Divide by 2
    current = a.load(std::memory_order_seq_cst);
    do {
        desired = current / 2;
    } while (!a.compare_exchange_strong(current, desired, std::memory_order_seq_cst));

    return a.load(std::memory_order_seq_cst);
}

int using_ROB(int x) {
    int a = x;
    a *= 8;
    a %= 9;
    a /= 2;
    return a;
}

double benchmark(int (*func)(int x),int x,int iterations) {
    zen::timer timer;
    timer.start();
    int result = 0;
    for (int i = 0; i < iterations; i++) {
        result += func(x);
    }
    zen::print(result,"\n");
    timer.stop();
    return timer.duration<zen::timer::nsec>().count();
}

int main(int argc,char* argv[]) {
    auto iterations = process_args(argc,argv);
    // Warm-up to  
    for (int i = 0; i < iterations; i++) {
        using_ROB(5);
        not_using_ROB(5);
    }

    double ROB_disable = benchmark(not_using_ROB,5,iterations);
    double ROB_enable = benchmark(using_ROB,5,iterations);
    double speedup = ((ROB_disable - ROB_enable)/ROB_disable)* 100;
    zen::print(zen::color::red(std::format("| {:<24} | {:>15.3f} s|\n","Not using ROB time: " , ROB_disable/1e9)));
    zen::print(zen::color::green(std::format("| {:<24} | {:>15.3f} s|\n","Using ROB time: " , ROB_enable/1e9)));
    zen::print(zen::color::yellow(std::format("| {:<24} | {:>15.3f}  s|\n","Speedup Factor:" , speedup)));

    return 0;
}