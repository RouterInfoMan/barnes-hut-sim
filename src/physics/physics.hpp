#pragma once

#include <cmath>
#include <vector>
#include <set>
#include "Body.hpp"

class PhysicsUtils {
public:
    static const double G = 6.67428;
    static sf::Vector2<double> getCenterOfMass(std::set <Body *> vec);
    static sf::Vector2<double> getCenterOfMass(Body *bod1, Body *bod2);
    static double totalMass(std::set <Body *> vec);
    static double totalMass(Body *bod1, Body *bod2);
    static sf::Vector2<double> getAccelFromForce(Body *bod, sf::Vector2<double> force);
    static sf::Vector2<double> getForce(Body *bod1, Body *bod2);
    static double norm(sf::Vector2 <double> v);
    static sf::Vector2<double> dotProduct(sf::Vector2<double>a, sf::Vector2<double>b);
};

struct barnes_hut_node {
    sf::Vector2<double> pos_top_corner, pos_bot_corner;
    Body *body;
    struct barnes_hut_node *children[4];

    ~barnes_hut_node() {
        bool ok = 1;

        for (int i = 0; i < 4; i++) {
            if (children[i] != NULL) {
                delete children[i];
                ok = 0;
            }
        }
        
        if (!ok)
            delete body;
    }
};

class BarnesHutTree {
private:
    double theta;
    barnes_hut_node *root;
    sf::Vector2<double> size;
    std::set <Body *> bodies;

    void insertBody(barnes_hut_node *root, Body *body, sf::Vector2 <double> bot, sf::Vector2 <double> top);
public:
    sf::Vector2<double> computeNetForce(Body *body);
    void walk(double dt);
    void constructTree(std::set <Body *> bodies);
    BarnesHutTree(std::set <Body *> bodies, sf::Vector2<double> size, double theta = 0.5) {
        this->bodies = bodies;
        this->size = size;
        this->theta = theta;
        this->constructTree(bodies);
    }
    


};