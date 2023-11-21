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

void AppEngine::eventHandler(sf::Event event) {
    if (event.type == sf::Event::Closed) {
        window->close();
        exit(0);
    }

    if (event.type == sf::Event::Resized) {
        // update the view to the new size of the window
        this->view.setSize(event.size.width * window_zoom, event.size.height * window_zoom);
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
        if (event.mouseWheel.delta > 0) {
            window_zoom *= zoom_per_scroll/1;
            this->view.zoom(zoom_per_scroll/1);
        } else {
            this->view.zoom(1/zoom_per_scroll);
            window_zoom *= 1/zoom_per_scroll;
        }
        // this->view.setCenter(sf::Vector2f(sf::Mouse::getPosition(*window)));
        this->window->setView(view);
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::N) {
            it++;
            if (it == this->bodies->end()) {
                it = this->bodies->begin();
            }
        }
        if (event.key.code == sf::Keyboard::F) {
            isViewFixed = !isViewFixed;
        }
    }
}
void AppEngine::setFramerateLimit(int frames) {
    this->window->setFramerateLimit(frames);
}

void AppEngine::universe_logic() {
        BarnesHutTree *bht = new BarnesHutTree(this->bodies, this->universe_size, 0);
        bht->walk(this->dt);
        delete bht;
}


void AppEngine::video_loop() {
    while (window->isOpen()) {
        universe_logic();

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
        if (isViewFixed) {
            view.setCenter(((BetterBody *)*it)->getShape()->getPosition());
            window->setView(view);
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
        double mass, size;
        utils::Vector2 pos, vel;
        int r, g, b;
        fin >> pos.x >> pos.y >> vel.x >> vel.y >> mass >> size >> r >> g >> b;

        BetterBody *sun = new BetterBody(mass, pos, vel);
        sf::CircleShape *sun_shp = new sf::CircleShape(size, 100);
        sun_shp->setFillColor(sf::Color(r, g, b));
        sun->setShape(sun_shp, &this->drawables);
        bodies->insert(sun);
    }
    it = bodies->begin();
    fin.close();
}

void AppEngine::start() {
    loadFromFile("planets.txt");
    video_loop();
}
