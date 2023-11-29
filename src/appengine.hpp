#pragma once

#include "physics/physics.hpp"
#include "threadpool.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <unistd.h>


class AppEngine {    
public:
    AppEngine(sf::Vector2f window_size, std::string window_name) {
        // Sfml
        this->window_size = window_size;
        this->window_name = window_name;
        this->view = sf::View(sf::FloatRect(0, 0, window_size.x, window_size.y));
        this->view.setCenter(0, 0);
        this->window = new sf::RenderWindow(sf::VideoMode(window_size.x, window_size.y), window_name);
        this->window->setView(this->view);
        // The actual good stuff
        this->bodies = new std::vector <Body *>;
        this->hours_passed = 0;
    }

    void start();
    void setFramerateLimit(int framerate);
    void video_loop();
    void universe_logic(utils::Vector2 size, double theta = 0.5, double G = 6.67430e-11, double epsilon = 0, double scale = 1e6);

private:
    sf::RenderWindow *window;
    std::vector <sf::Drawable *> drawables;
    sf::Vector2f window_size;
    std::string window_name;
    sf::View view;
    int hours_passed;
    float window_zoom = 1;
    float zoom_per_scroll = 0.95f;
    bool mouseMoving = false;
    bool isViewFixed = false;
    sf::Vector2i oldMousePos;
    std::vector<Body *>::iterator it;

    void eventHandler(sf::Event event);

    std::vector <Body *> *bodies;
    // Size of universe (real, i calculated it)
    // One day
    const double dt = 60;
    // Scale
    double scale = 1e6;

    sf::Thread *loadFromFile(std::string path);
    sf::Thread *random_bodies();
};