#include "physics/physics.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex> 

auto now() { return std::chrono::steady_clock::now(); }
 
auto awake_time()
{
    using std::chrono::operator""ms;
    return now() + 5ms;
}
void func(BarnesHutTree *tree) {
    while (1) {
        const auto start{now()};
        std::this_thread::sleep_until(awake_time());
        tree->walk(0.005);
    }
}
int main() {
    
    return 0;
}
