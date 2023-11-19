#pragma once
#include "utils.hpp"


class Body {
private:
    double mass;
    utils::Vector2 pos;
    utils::Vector2 velocity;
    utils::Vector2 acceleration;
public:
    Body(double mass, utils::Vector2 pos, utils::Vector2 vel) {
        this->mass = mass;
        this->pos = pos;
        this->velocity = vel;
        this->acceleration = utils::Vector2(0, 0);
    }
    Body(double mass, utils::Vector2 pos) {
        this->mass = mass;
        this->pos = pos;
        this->velocity = this->acceleration = utils::Vector2(0, 0);
    }
    utils::Vector2 getPos();
    void setPos(utils::Vector2 pos);
    double getMass();
    void setMass(double mass);
    utils::Vector2 getVelocity();
    void setVelocity(utils::Vector2 velocity);
    utils::Vector2 getAcceleration();
    void setAcceleration(utils::Vector2 acceleration);

    void updateMovement(double dt);
};

