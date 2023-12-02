#include "physics.hpp"



utils::Vector2 PhysicsUtils::getCenterOfMass(std::set <Body *> vec) {
    double x = 0, y = 0;
    double total_mass = 0;
    for (auto& bod : vec) {
        x += bod->getPosition().x * bod->getMass();
        y += bod->getPosition().y * bod->getMass();
        total_mass += bod->getMass();
    }
    if (total_mass == 0) {
        return {7, 7};
    }
    return utils::Vector2 (x / total_mass, y / total_mass);
}

utils::Vector2  PhysicsUtils::getCenterOfMass(Body *bod1, Body *bod2) {
    return (bod1->getPosition() * bod1->getMass() + bod2->getMass() * bod2->getPosition()) / (bod1->getMass() + bod2->getMass());
}
utils::Vector2 PhysicsUtils::getCenterOfMass(double mass1, utils::Vector2 pos1, Body* bod2) {
    return (mass1 * pos1 + bod2->getMass() * bod2->getPosition()) / (mass1 + bod2->getMass());
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
double PhysicsUtils::totalMass(double mass1, Body *bod2) {
    return mass1 + bod2->getMass();
}


utils::Vector2  PhysicsUtils::getAccelFromForce(Body *bod, utils::Vector2  force) {
    return utils::Vector2 (force.x/bod->getMass(), force.y/bod->getMass());
}

// Force applied by bod2 to bod1
utils::Vector2  PhysicsUtils::getForce(Body *bod1, Body *bod2, double G, double epsilon/*=0*/) {
    utils::Vector2  r = bod1->getPosition() - bod2->getPosition();
    
    utils::Vector2 force(1, 1);
    double rn = r.norm();
    force *= G * bod1->getMass() * bod2->getMass() / (rn * rn + epsilon);

    r /= rn;
    
    force *= r;
    return force;
}
 bool BarnesHutTree::is_purgable(utils::Vector2 pos) {
    utils::Vector2 other = this->corner + this->size;
    if (pos.x < corner.x || pos.y < corner.y) {
        return true;
    }
    if (pos.x > other.x || pos.y > other.y) {
        return true;    
    }
    return false;
}
size_t chooseQuad(utils::Vector2  pos, utils::Vector2  top, utils::Vector2  half_size) {
    utils::Vector2  mid = top + half_size;
    if (pos.x < mid.x) {
        if (pos.y < mid.y) {
            return 0;
        }
        return 2;
    }
    if (pos.y <= mid.y)
        return 1;
    return 3;
}
utils::Vector2 getPosQuad(size_t quad, utils::Vector2  top, utils::Vector2  half_size) {
    if (quad == 0)
        return top;

    if (quad == 1)
        return utils::Vector2 (top.x + half_size.x, top.y);

    if (quad == 2)
        return utils::Vector2 (top.x, top.y + half_size.y);

    return utils::Vector2 (top.x + half_size.x, top.y + half_size.y);
}
void BarnesHutTree::insertBody(barnes_hut_node* &it, Body *body, utils::Vector2 top, utils::Vector2 size) {
    if (it == NULL) {
        it = new barnes_hut_node(top, size, false, body);
        return;
    }
    utils::Vector2  half_size = size * 0.5;
    size_t bquad = chooseQuad(body->getPosition(), top, half_size);

    // External node
    if (it->internal == false) {
        Body *c = it->body;
        size_t cquad = chooseQuad(c->getPosition(), top, half_size);

        it->internal = true;
        it->body = NULL;
        it->mass = PhysicsUtils::totalMass(c, body);
        it->center = PhysicsUtils::getCenterOfMass(c, body);
        it->max_rad = std::max(c->getRadius(), body->getRadius());
        
        insertBody(it->children[cquad], c, getPosQuad(cquad, top, half_size), half_size);
        insertBody(it->children[bquad], body, getPosQuad(bquad, top, half_size), half_size);
        return;
    }

    // Internal node
    it->center = PhysicsUtils::getCenterOfMass(it->mass, it->center, body);
    it->mass = PhysicsUtils::totalMass(it->mass, body);
    it->max_rad = std::max(it->max_rad, body->getRadius());

    insertBody(it->children[bquad], body, getPosQuad(bquad, top, half_size), half_size);
}

void BarnesHutTree::constructTree() {
    for (auto &x: *(this->bodies)) {
        insertBody(this->root, x, this->corner, this->size);
    }
}
static int count = 0;
utils::Vector2 BarnesHutTree::computeNetForceHelper(barnes_hut_node *it, Body *body, double epsilon) {
    count++;
    if (it == NULL) {
        return utils::Vector2(0, 0);
    }
    if (it->body == body) {
        return utils::Vector2(0, 0);
    }
    if (it->internal == false) {
        return PhysicsUtils::getForce(it->body, body, this->G, epsilon);
    }

    double s = it->size.norm();
    double d = (it->center - body->getPosition()).norm();
    if (s / d < theta) {
        Body bd(it->mass, it->center);
        return PhysicsUtils::getForce(&bd, body, this->G, epsilon);
    } 

    utils::Vector2 sum(0, 0);
    for (int i = 0; i < 4; i++) {
        if (it->children[i] != NULL) {
            sum += computeNetForceHelper(it->children[i], body, epsilon);
        }
    }

    return sum;
}

void BarnesHutTree::applyNetForce(Body *body) {
    utils::Vector2 force = computeNetForceHelper(this->root, body, epsilon);
    body->setAcceleration(force / body->getMass());
}

void BarnesHutTree::plastic_collision(Body *a, Body * b) {
    if (a == b) {
        return;
    }
    utils::Vector2 distVec = a->getPosition() - b->getPosition();
    double dist = distVec.norm();
    double minDist = a->getRadius() + b->getRadius();
    double amass = a->getMass(), bmass = b->getMass();
    utils::Vector2 avel = a->getVelocity(), bvel = b->getVelocity();
   
    if (dist > minDist)
        return;
    // std::cout << dist << " " << minDist << "\n";
    Body *bigger, *smaller;
    if (a->getMass() > b->getMass()) {
        bigger = a;
        smaller = b;
    } else {
        bigger = b;
        smaller = a;
    }
    this->bodies->erase(std::remove(this->bodies->begin(), this->bodies->end(), smaller), this->bodies->end());

    double newMass = bigger->getMass() + smaller->getMass();
    bigger->setVelocity((bigger->getVelocity() * bigger->getMass() + smaller->getMass() * smaller->getVelocity())/newMass);
    double sr = smaller->getRadius(), br = bigger->getRadius();
    bigger->setRadius(pow(sr * sr * sr + br * br * br, 1.0f/3));
    bigger->setMass(newMass);

}
void BarnesHutTree::elastic_collision(Body *a, Body * b) {
    if (a == b) {
        return;
    }
    utils::Vector2 distVec = a->getPosition() - b->getPosition();
    double dist = distVec.norm();
    double minDist = a->getRadius() + b->getRadius();
    double amass = a->getMass(), bmass = b->getMass();
    utils::Vector2 avel = a->getVelocity(), bvel = b->getVelocity();
    if (dist > minDist)
        return;
    
    distVec /= dist;

    utils::Vector2 mtd = distVec * (minDist - dist);

    a->setPosition(a->getPosition() + mtd * bmass/(bmass + amass));
    b->setPosition(b->getPosition() - mtd * amass/(bmass + amass));
    
    utils::Vector2 apos = a->getPosition(), bpos = b->getPosition();

    utils::Vector2 va = avel - 2 * bmass/(amass + bmass) * (avel - bvel).dot(apos - bpos)/(apos-bpos).norm2() * (apos - bpos);
    utils::Vector2 vb = bvel - 2 * amass/(amass + bmass) * (bvel - avel).dot(bpos - apos)/(bpos-apos).norm2() * (bpos - apos);


    /// Losing energy
    a->setVelocity(va * 0.90);
    b->setVelocity(vb * 0.90);

}
void BarnesHutTree::collide(barnes_hut_node *it, Body *body) {
    if (!it->internal) {
        this->plastic_collision(it->body, body);
        return;
    }
    for (int i = 0; i < 4; i++) {
        if (it->children[i]) {
            if (it->children[i]->internal) {
                double dist = body->getRadius() + it->children[i]->max_rad;
                utils::Vector2 bpos = body->getPosition();
                bool outside_range = bpos.x + dist < it->children[i]->top_corner.x || bpos.x - dist > it->children[i]->top_corner.x + it->children[i]->size.x
                                  || bpos.y + dist < it->children[i]->top_corner.y || bpos.y - dist > it->children[i]->top_corner.y + it->children[i]->size.y;
                if (!outside_range) {
                    this->collide(it->children[i], body);
                }
            } else {
                collide(it->children[i], body);
            }
        }
    }
}

void BarnesHutTree::walk(double dt) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    this->constructTree();
    for (auto &x : *(this->bodies)) {
        this->collide(this->root, x);
    }
    auto t1 = high_resolution_clock::now();
    for (auto &x : *(this->bodies)) {
        // this->applyNetForce(x);
        this->pool->QueueJob([&] {this->applyNetForce(x);});
    }
    while(this->pool->busy());
    auto t2 = high_resolution_clock::now();

    duration<double, std::milli> ms_double = t2 - t1;
    
    if (ms_double.count() < 10) {
        usleep(10000 - ms_double.count() * 1000);
    }
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    std::cout << ms_double.count() << "ms\n";
    for (auto &x: *(this->bodies)) {
        x->updateMovement(dt);
    }

    delete this->root;
    for (auto x = this->bodies->begin(); x != this->bodies->end(); ) {
        if (this->is_purgable((*x)->getPosition())) {
            delete *x;
            x = this->bodies->erase(x);
            if (x == this->bodies->end()) {
                break;
            }
        } else {
            ++x;
        }
    }
    this->root = NULL;
}

void BarnesHutTree::walkCUDA(float dt) {
    // this->constructTree();
    // for (auto &x : *(this->bodies)) {
    //     this->collide(this->root, x);
    // }

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;


    auto t1 = high_resolution_clock::now();

    

    size_t length = this->bodies->size();
    CUDABody_t *bodies = (CUDABody_t *)malloc(sizeof(CUDABody_t) * length);
    for (int i = 0; i < length; i++) {
        bodies[i].pos = make_float2((*this->bodies)[i]->getPosition().x, (*this->bodies)[i]->getPosition().y);
        bodies[i].vel = make_float2((*this->bodies)[i]->getVelocity().x, (*this->bodies)[i]->getVelocity().y);
        bodies[i].mass = (*this->bodies)[i]->getMass();
    }
    
    walkCUDADevice(bodies, length, this->G, dt);
    for (int i = 0; i < length; i++) {
        (*this->bodies)[i]->setPosition(utils::Vector2(bodies[i].pos.x, bodies[i].pos.y));
        (*this->bodies)[i]->setVelocity(utils::Vector2(bodies[i].vel.x, bodies[i].vel.y));
    }

    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    std::cout << ms_double.count() << "ms\n";
    // sleep(1);

    // delete this->root;
    // for (auto x = this->bodies->begin(); x != this->bodies->end(); ) {
    //     if (this->is_purgable((*x)->getPosition())) {
    //         delete *x;
    //         x = this->bodies->erase(x);
    //         if (x == this->bodies->end()) {
    //             break;
    //         }
    //     } else {
    //         ++x;
    //     }
    // }
    // this->root = NULL;
    // free(bodies);
}