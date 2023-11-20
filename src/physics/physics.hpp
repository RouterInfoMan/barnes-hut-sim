#pragma once

#include <cmath>
#include <vector>
#include <set>
#include <iostream>
#include "body.hpp"


namespace PhysicsUtils {
    utils::Vector2 getCenterOfMass(std::set <Body *> vec);
    utils::Vector2 getCenterOfMass(Body *bod1, Body *bod2);
    double totalMass(std::set <Body *> vec);
    double totalMass(Body *bod1, Body *bod2);
    utils::Vector2 getAccelFromForce(Body *bod, utils::Vector2 force);
    utils::Vector2 getForce(Body *bod1, Body *bod2);
};

struct barnes_hut_node {
    utils::Vector2  top_corner, size;
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
    utils::Vector2  corner, size;
    std::set <Body *> *bodies;

    void insertBody(barnes_hut_node *&root, Body *body, utils::Vector2 bot, utils::Vector2 top);
    void computeNetForceHelper(utils::Vector2  &sum, barnes_hut_node *it, Body *body);
    bool is_purgable(utils::Vector2 pos);
public:
    utils::Vector2  computeNetForce(Body *body);
    void walk(double dt);
    void constructTree();
    BarnesHutTree(std::set <Body *> *&bodies, utils::Vector2  size, double theta = 0.5) {
        this->bodies = bodies;
        this->size = size;
        this->theta = theta;
        this->root = NULL;
        this->corner = utils::Vector2(0, 0) - size * 0.5; 
    }
};