#include "physics.hpp"

sf::Vector2<double> PhysicsUtils::getCenterOfMass(std::set <Body *> vec) {
    double x = 0, y = 0;
    double total_mass = 0;
    for (auto& bod : vec) {
        x += bod->getPos().x * bod->getMass();
        y += bod->getPos().y * bod->getMass();
        total_mass += bod->getMass();
    }
    if (total_mass == 0) {
        exit(1);
    }
    return sf::Vector2<double>(x / total_mass, y / total_mass);
}

sf::Vector2<double> PhysicsUtils::getCenterOfMass(Body *bod1, Body *bod2) {
    return PhysicsUtils::dotProduct(bod1->getMass() * bod1->getPos(), bod2->getMass() * bod2->getPos()) / (bod1->getMass() + bod2->getMass());
}

double PhysicsUtils::totalMass(std::set <Body *> vec) {
    double total_mass = 0;
    for (auto& bod : vec) {
        total_mass += bod->getMass();
    }
    return total_mass;
}
double PhysicsUtils::totalMass(Body *bod1, Body *bod2) {
    return bod1->getMass() + bod2->getMass();
}


sf::Vector2<double> PhysicsUtils::getAccelFromForce(Body *bod, sf::Vector2<double> force) {
    return sf::Vector2<double>(force.x/bod->getMass(), force.y/bod->getMass());
}

double PhysicsUtils::norm(sf::Vector2<double> v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2<double> PhysicsUtils::dotProduct(sf::Vector2<double>a, sf::Vector2<double>b) {
    return sf::Vector2<double>(a.x * b.x, a.y * b.y);
}
// Force applied by bod2 to bod1
sf::Vector2<double> PhysicsUtils::getForce(Body *bod1, Body *bod2) {
    sf::Vector2<double> r = bod2->getPos() - bod1->getPos();
    sf::Vector2<double> force = r;
    double rn = PhysicsUtils::norm(r);
    force *= PhysicsUtils::G * bod1->getMass() * bod2->getMass() / (rn * rn);

    r /= PhysicsUtils::norm(r);
    PhysicsUtils::dotProduct(force, r);
    return force;
}

size_t chooseQuadrant(sf::Vector2<double> pos, sf::Vector2<double> top, sf::Vector2<double> bot) {
    sf::Vector2<double> r = bot - top;
    if (pos.x - top.x < r.x / 2) {
        if (pos.y - top.y < r.y / 2) {
            return 0;
        }
        return 2;
    }
    if (pos.y - top.y < r.y / 2) {
        return 1;
    }
    return 3;
}

void BarnesHutTree::insertBody(barnes_hut_node *it, Body *body, sf::Vector2 <double> bot, sf::Vector2 <double> top) {
    if (it == NULL) {
        it = new barnes_hut_node;
        it->pos_top_corner = top;
        it->pos_bot_corner = bot;
        it->body = body;

        for (int i =)

        return;
    }
    // External node
    if (std::find(this->bodies.begin(), this->bodies.end(), it->body) != this->bodies.end()) {
        
    }

}



void BarnesHutTree::constructTree(std::set <Body *> bodies) {
    for (auto &x: bodies) {
        insertBody(this->root, x, sf::Vector2<double>(0, 0), sf::Vector2<double>(this->size.x, this->size.y));
    }
}
sf::Vector2<double> BarnesHutTree::computeNetForce(Body *body) {

}