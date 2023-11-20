#pragma once

#include "physics/physics.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>

class BetterBody : public Body {
private:
    sf::CircleShape *shp;
public:
    BetterBody(double mass, utils::Vector2 pos, utils::Vector2 vel = utils::Vector2(0, 0))
    : Body(mass, pos, vel) {
        shp = NULL;
    }
    void setShape(sf::CircleShape *shp, std::vector <sf::Drawable *> *drawables) {
        if (shp)
            delete this->shp;
        drawables->erase(std::remove(drawables->begin(), drawables->end(), shp), drawables->end());
        this->shp = shp;
        this->shp->setOrigin(this->shp->getRadius(), this->shp->getRadius());
        drawables->push_back(shp);
    }
    sf::CircleShape *getShape() {
        return this->shp;
    }
    void updateShapePos(double zoom) {
        this->shp->setPosition(this->getPos().x / zoom, this->getPos().y / zoom);
    }
};


class AppEngine {
private:
    sf::RenderWindow *window;
    std::vector <sf::Drawable *> drawables;
    sf::Vector2f window_size;
    std::string window_name;
    sf::View view;
    float window_zoom = 1;
    bool mouseMoving = false;
    sf::Vector2i oldMousePos;

    void eventHandler(sf::Event event);

    std::set <Body *> *bodies;
    // Size of universe (real, i calculated it)
    const utils::Vector2 universe_size = {1e14, 1e14};
    // One day
    const double dt = 3600;
    // Scale
    const double scale = 1e7;

    sf::Vector2f normalize_in_view(utils::Vector2 pos) {
        pos /= scale;
        return sf::Vector2f(pos.x, pos.y);
    }
    void loadFromFile(std::string path);
    
public:

    AppEngine(sf::Vector2f window_size, std::string window_name) {
        // Sfml
        this->window_size = window_size;
        this->window_name = window_name;
        this->view = sf::View(sf::FloatRect(0, 0, window_size.x, window_size.y));
        this->view.setCenter(0, 0);
        window = new sf::RenderWindow(sf::VideoMode(window_size.x, window_size.y), window_name);
        window->setView(this->view);
        // The actual good stuff
        bodies = new std::set <Body *>;
    }

    void start();
    void setFramerateLimit(int framerate);
    void video_loop();
    void universe_logic();

};