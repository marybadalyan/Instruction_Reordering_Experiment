#include <iostream>
#include <vector>
#include "kaizen.h"  
#include <format>

// Parse command-line arguments
int process_args(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);
    auto iter_options = args.get_options("--iterations");

    if (iter_options.empty()) {
        zen::print("Error: --iterations  argument is absent, using default:iterations=1'000'000\n");
        return {1000000}; // Increased defaults
    }
    return std::stoi(iter_options[0]);
}

int not_using_ROB() {
    volatile int a = 5, b = 3, c = 7, d = 2;
    a *= d;
    b %= a;
    c += b;
    d *= c;
    return a + b + c + d;
}

int using_ROB() {
    volatile int a = 5, b = 3, c = 7, d = 2;
    a *= 8;
    b %= 9;
    c += 4;
    d *= 3;
    return a + b + c + d;
}

double benchmark(int (*func)(),int iterations) {
    zen::timer timer;
    timer.start();
    int result = 0;
    for (int i = 0; i < iterations; i++) {
        result += func();
    }
    timer.stop();
    return timer.duration<zen::timer::nsec>().count();
}

int main(int argc,char* argv[]) {
    auto iterations = process_args(argc,argv);
    // Warm-up to  
    for (int i = 0; i < iterations; i++) {
        using_ROB();
        not_using_ROB();
    }

    double ROB_disable = benchmark(not_using_ROB,iterations);
    double ROB_enable = benchmark(using_ROB,iterations);

    zen::print(zen::color::red(std::format("| {:<24} | {:>12.3f} ns|\n","Not using ROB time: " , ROB_disable)));
    zen::print(zen::color::green(std::format("| {:<24} | {:>12.3f} ns|\n","Using ROB time: " , ROB_enable)));
    zen::print(zen::color::yellow(std::format("| {:<24} | {:>15.4f}|\n","Speedup Factor:" , ROB_enable / ROB_disable)));

    return 0;
}