#include "appengine.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex> 

int main(int argc, char *argv[]) {
    AppEngine *app = NULL;
    if (argc == 1) {
        app = new AppEngine({1920, 1080}, "catastrophes");
    }
    if (app == NULL) {
        return 1;
    }
    app->start();
    return 0;
}
