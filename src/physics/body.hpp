#pragma once

#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "utils.hpp"


class Body {
public:
    Body(double mass, double radius, utils::Vector2 pos, utils::Vector2 vel, sf::CircleShape shp) {
        this->radius = radius;
        this->mass = mass;
        this->density = mass / (radius * radius * M_PI);
        this->pos = pos;
        this->velocity = vel;
        this->acceleration = utils::Vector2(0, 0);

        this->shp = shp;
        shp.setOutlineThickness(2);
    }
    Body(double mass, utils::Vector2 pos) {
        this->mass = mass;
        this->pos = pos;
    }
    utils::Vector2 getPosition();
    void setPosition(utils::Vector2 pos);

    double getMass();
    void setMass(double mass);

    utils::Vector2 getVelocity();
    void setVelocity(utils::Vector2 velocity);

    utils::Vector2 getAcceleration();
    void setAcceleration(utils::Vector2 acceleration);

    sf::CircleShape getShape();
    void setShape(sf::CircleShape shp);
    void updateShape(double scale);

    double getRadius();
    void setRadius(double radius);

    void updateMovement(double dt);

private:
    sf::CircleShape shp;
    double mass;
    utils::Vector2 pos;
    utils::Vector2 velocity;
    utils::Vector2 acceleration;
    double radius;
    double density; // kg/m^2
};

