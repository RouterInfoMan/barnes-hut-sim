#include "physics/physics.hpp"
#include "appengine.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex> 

int main() {
    
    AppEngine app({1920, 1080}, "catastrophes");
    app.start();

    return 0;
}
