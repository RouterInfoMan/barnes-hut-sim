#include "Body.hpp"

sf::Vector2<double> Body::getPos() {
    return this->pos;
}
void Body::setPos(sf::Vector2<double> pos) {
    this->pos = pos;
}
double Body::getMass() {
    return this->mass;
}
void Body::setMass(double mass) {
    this->mass = mass;
}
sf::Vector2<double> Body::getVelocity() {
    return this->velocity;
}
void Body::setVelocity(sf::Vector2<double> velocity) {
    this->velocity = velocity;
}
sf::Vector2<double> Body::getAcceleration() {
    return this->acceleration;
}
void Body::setAcceleration(sf::Vector2<double> acceleration) {
    this->acceleration = acceleration;
}

void Body::updateMovement(double dt) {
    this->pos.x = this->pos.x + this->velocity.x * dt + this->acceleration.x / 2 * dt * dt;
    this->pos.y = this->pos.y + this->velocity.y * dt + this->acceleration.y / 2 * dt * dt;
    this->velocity.x = this->velocity.x + this->acceleration.x * dt;
    this->velocity.y = this->velocity.y + this->acceleration.y * dt;
}