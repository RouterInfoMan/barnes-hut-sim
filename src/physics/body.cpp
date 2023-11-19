#include "body.hpp"

utils::Vector2 Body::getPos() {
    return this->pos;
}
void Body::setPos(utils::Vector2 pos) {
    this->pos = pos;
}
double Body::getMass() {
    return this->mass;
}
void Body::setMass(double mass) {
    this->mass = mass;
}
utils::Vector2 Body::getVelocity() {
    return this->velocity;
}
void Body::setVelocity(utils::Vector2 velocity) {
    this->velocity = velocity;
}
utils::Vector2 Body::getAcceleration() {
    return this->acceleration;
}
void Body::setAcceleration(utils::Vector2 acceleration) {
    this->acceleration = acceleration;
}
void Body::updateMovement(double dt) {
    this->pos = this->pos + this->velocity * dt + this->acceleration * dt * dt / 2;
    this->velocity = this->velocity + this->acceleration * dt;

    // Goodbye forces
    this->acceleration = utils::Vector2(0, 0);
}