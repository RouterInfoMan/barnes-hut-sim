#include "physics/physics.hpp"
#include "appengine.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <unistd.h>

auto now() { return std::chrono::steady_clock::now(); }

auto awake_time()
{
    using std::chrono::operator""ms;
    return now() + 10ms;
}

void AppEngine::eventHandler(sf::Event event) {
    if (event.type == sf::Event::Closed) {
        window->close();
        exit(0);
    }

    if (event.type == sf::Event::Resized) {
        // update the view to the new size of the window
        this->view.setSize(event.size.width, event.size.height);
        window->setView(this->view);
    }
   
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            oldMousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            if (!mouseMoving) {
                mouseMoving = true;
            }
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left && mouseMoving) {
            mouseMoving = false;
            sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2i change = oldMousePos - mousePos;
            this->view.move(change.x * window_zoom, change.y * window_zoom);
            this->window->setView(this->view);

        }
    }

    if (event.type == sf::Event::MouseWheelMoved) {
        std::cout << "delta" << std::endl;
        if (event.mouseWheel.delta > 0) {
            window_zoom *= 99.f/100;
            this->view.zoom(99.f/100);
        } else {
            this->view.zoom(100.f/99);
            window_zoom *= 100.f/99;
        }
        // this->view.setCenter(sf::Vector2f(sf::Mouse::getPosition(*window)));
        this->window->setView(view);
        
    }
}
void AppEngine::setFramerateLimit(int frames) {
    this->window->setFramerateLimit(frames);
}

void AppEngine::universe_logic() {
    sleep(5);
    while(1) {
        std::cout << "wassup\n\n" << std::endl;
        const auto start{now()};
        std::this_thread::sleep_until(awake_time());
        BarnesHutTree *bht = new BarnesHutTree(this->bodies, this->universe_size, 0);
        bht->walk(this->dt);
        delete bht;
    }
}


void AppEngine::video_loop() {
    while (window->isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window->pollEvent(event)) {
            this->eventHandler(event);
        }
        if (mouseMoving) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            sf::Vector2i change = oldMousePos - mousePos;

            sf::View temp = this->view;
            temp.move(change.x * window_zoom, change.y * window_zoom);
            this->window->setView(temp);
        }


        // clear the window with black color
        window->clear(sf::Color::Black);
        for (auto &x : *bodies) {
            ((BetterBody *)x)->updateShapePos(this->scale);
        }


        for (auto &x : drawables) {
            window->draw(*x);
        }
        window->display();
    }
}

void AppEngine::loadFromFile(std::string path) {
    std::ifstream fin;
    fin.open(path);
    int count;
    fin >> count;
    // count = 7;
    for (int i = 0; i < count; i++) {
        double mass;
        utils::Vector2 pos, vel;
        int size, r, g, b;
        fin >> pos.x >> pos.y >> vel.x >> vel.y >> mass >> size >> r >> g >> b;
        BetterBody *sun = new BetterBody(mass, pos, vel);
        sf::CircleShape *sun_shp = new sf::CircleShape(size, 100);
        sun_shp->setFillColor(sf::Color(r, g, b));
        sun->setShape(sun_shp, &this->drawables);
        bodies->insert(sun);
    }


    fin.close();
}

void AppEngine::start() {
    loadFromFile("planets.txt");


    std::cout << bodies->size() << std::endl;

    sf::Thread logic(&AppEngine::universe_logic, this);
    logic.launch();
    video_loop();
    
}
