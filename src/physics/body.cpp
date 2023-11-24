#include "body.hpp"

utils::Vector2 Body::getPosition() {
    return this->pos;
}
void Body::setPosition(utils::Vector2 pos) {
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

sf::CircleShape Body::getShape() {
    return this->shp;
}
void Body::setShape(sf::CircleShape shp) {
    this->shp = shp;
}
void Body::updateShape(double scale) {
    this->shp.setRadius(this->radius/scale);
    this->shp.setOrigin(this->shp.getRadius(), this->shp.getRadius());
    this->shp.setPosition(this->pos.x / scale, this->pos.y / scale);
}

double Body::getRadius() {
    return this->radius;
}
void Body::setRadius(double radius) {
    this->radius = radius;
}

void Body::updateMovement(double dt) {
    //this->pos = this->pos + this->velocity * dt + this->acceleration * dt * dt / 2;
    this->velocity = this->velocity + this->acceleration * dt;
    this->pos = this->pos + this->velocity * dt;
    // Goodbye forces
    this->acceleration = utils::Vector2(0, 0);
}