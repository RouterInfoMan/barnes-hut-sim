#pragma once
#include <SFML/System.hpp>


class Body {
private:
    double mass;
    sf::Vector2<double> pos;
    sf::Vector2<double> velocity;
    sf::Vector2<double> acceleration;
public:
    Body(double mass, sf::Vector2<double> pos) {
        this->mass = mass;
        this->pos = pos;
        this->velocity = this->acceleration = sf::Vector2<double>({0, 0});
    }
    sf::Vector2<double> getPos();
    void setPos(sf::Vector2<double> pos);
    double getMass();
    void setMass(double mass);
    sf::Vector2<double> getVelocity();
    void setVelocity(sf::Vector2<double> velocity);
    sf::Vector2<double> getAcceleration();
    void setAcceleration(sf::Vector2<double> acceleration);

    void updateMovement(double dt);
};

