#pragma once

#include <cmath>
#include <vector>
#include <set>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include "cuda.hpp"
#include "body.hpp"
#include "../threadpool.hpp"


namespace PhysicsUtils {
    utils::Vector2 getCenterOfMass(std::set <Body *> vec);
    utils::Vector2 getCenterOfMass(Body *bod1, Body *bod2);
    utils::Vector2 getCenterOfMass(double mass1, utils::Vector2 pos1, Body *bod2);
    double totalMass(std::set <Body *> vec);
    double totalMass(Body *bod1, Body *bod2);
    double totalMass(double mass1, Body *bod2);
    utils::Vector2 getAccelFromForce(Body *bod, utils::Vector2 force);
    utils::Vector2 getForce(Body *bod1, Body *bod2, double G, double epsilon = 0);
};

struct barnes_hut_node {
    utils::Vector2 top_corner, size;
    utils::Vector2 center;
    double max_rad;
    double mass;
    Body *body;
    struct barnes_hut_node *children[4];

    bool internal;

    barnes_hut_node(utils::Vector2 top_corner, utils::Vector2 size, bool internal=true, Body *body = NULL) {
        this->top_corner = top_corner;
        this->size = size;
        this->internal = internal;
        this->body = body;
        for (int i = 0; i < 4; i++) {
            this->children[i] = NULL;
        }
    }

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
public:
    void walk(double dt);
    void walkCUDA(float dt);
    void constructTree();
    BarnesHutTree(std::vector <Body *> *&bodies, utils::Vector2  size, double theta = 0.5, double G = 6.67430e-11, double epsilon = 0) {
        this->bodies = bodies;
        this->size = size;
        this->root = NULL;
        this->corner = utils::Vector2(0, 0) - size * 0.5;
        this->theta = theta;
        this->G = G;
        this->epsilon = epsilon;

        this->pool = new ThreadPool();
        this->pool->Start();
    }
    ~BarnesHutTree() {
        this->pool->Stop();
        delete this->pool;
    }

    private:
    /// System properties
    double theta;
    double epsilon;
    double G;
    utils::Vector2  corner, size;
    std::vector <Body *> *bodies;
    
    barnes_hut_node *root;
    ThreadPool *pool;
    void collide(barnes_hut_node *it, Body *body);
    void plastic_collision(Body *a, Body * b);
    void elastic_collision(Body *a, Body * b);
    void applyNetForce(Body *body);
    void insertBody(barnes_hut_node *&root, Body *body, utils::Vector2 bot, utils::Vector2 top);
    utils::Vector2 computeNetForceHelper(barnes_hut_node *it, Body *body, double epsilon);
    bool is_purgable(utils::Vector2 pos);
};