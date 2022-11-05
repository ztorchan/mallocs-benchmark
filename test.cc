#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

int main() {
    auto clock_start = clock();
    auto chrono_clock_start = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(std::chrono::seconds(5));
    for (int i = 0; i <= 1000000; i++);

    auto clock_end = clock();
    auto chrono_clock_end = std::chrono::steady_clock::now();

    std::cout << (clock_end - clock_start) * (100000.0 / CLOCKS_PER_SEC) << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(chrono_clock_end - chrono_clock_start).count() << std::endl;
    
    return 0;
}