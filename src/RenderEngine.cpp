#include "physics/physics.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>

class LogicEngine {
public:
    void handleKey(sf::Keyboard::key key) {

    } 
};

class RenderEngine {
private:
    sf::RenderWindow *window;
    std::vector <sf::Drawable> drawables;
    sf::Vector2f window_size;
    std::string window_name;
    LogicEngine *logic;
public:
    RenderEngine(sf::Vector2f window_size, std::string window_name) {
        this->window_size = window_size;
        this->window_name = window_name;

        window = new sf::RenderWindow(sf::VideoMode(window_size), window_name);
    }
    void setFramerateLimit(int frames) {
        this->window->setFramerateLimit(frames);
    }

    void setLogicEngine(LogicEngine *logic) {
        this->logic = logic;
    }

    void loop() {
        while (window->isOpen())
        {
            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;
            while (window->pollEvent(event))
            {
                // "close requested" event: we close the window
                if (event.type == sf::Event::KeyPressed) {
                    logic.handleKey(event.key.code);
                }
                if (event.type == sf::Event::Closed)
                    window->close();
            }

            // clear the window with black color
            window->clear(sf::Color::Black);
            for (auto &x : drawables) {
                window->draw(x);
            }
        
            window->display();
        }
    }
    sf::Thread loopThread() {
        return sf::Thread(this->loop);
    }

};