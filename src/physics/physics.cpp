#include "physics.hpp"

utils::Vector2 PhysicsUtils::getCenterOfMass(std::set <Body *> vec) {
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
    return utils::Vector2 (x / total_mass, y / total_mass);
}

utils::Vector2  PhysicsUtils::getCenterOfMass(Body *bod1, Body *bod2) {
    return (bod1->getPos() * bod1->getMass() + bod2->getMass() * bod2->getPos()) / (bod1->getMass() + bod2->getMass());
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


utils::Vector2  PhysicsUtils::getAccelFromForce(Body *bod, utils::Vector2  force) {
    return utils::Vector2 (force.x/bod->getMass(), force.y/bod->getMass());
}

// Force applied by bod2 to bod1
utils::Vector2  PhysicsUtils::getForce(Body *bod1, Body *bod2) {
    utils::Vector2  r = bod1->getPos() - bod2->getPos();
    
    utils::Vector2  force(1, 1);
    double rn = r.norm();
    force *= utils::G * bod1->getMass() * bod2->getMass() / (rn * rn);

    r /= r.norm();
    
    force *= r;
    return force;
}

size_t chooseQuad(utils::Vector2  pos, utils::Vector2  top, utils::Vector2  size) {
    utils::Vector2  mid = top + size * 0.5;
    if (pos.x < mid.x) {
        if (pos.y < mid.y) {
            return 0;
        }
        return 2;
    }
    if (pos.y < mid.y) {
        return 1;
    }
    return 3;
}
utils::Vector2  getPosQuad(size_t quad, utils::Vector2  top, utils::Vector2  half_size) {
    if (quad == 0) {
        return top;
    }
    if (quad == 1) {
        return utils::Vector2 (top.x + half_size.x, top.y);
    }
    if (quad == 2) {
        return utils::Vector2 (top.x, top.y + half_size.y);
    }
    return utils::Vector2 (top.x + half_size.x, top.y + half_size.y);
}
void BarnesHutTree::insertBody(barnes_hut_node* &it, Body *body, utils::Vector2 top, utils::Vector2 size) {
    if (it == NULL) {
        it = new barnes_hut_node;
        it->top_corner = top;
        it->size = size;
        it->body = body;
        
        for (int i = 0; i < 4; i++) {
            it->children[i] = NULL;
        }
        return;
    }
    utils::Vector2  half_size = size * 0.5;
    
    size_t bquad = chooseQuad(body->getPos(), top, size);
    // External node
    if (this->bodies->find(it->body) != this->bodies->end()) {
        Body *c = it->body;
        size_t cquad = chooseQuad(it->body->getPos(), top, size);
        it->top_corner = top;
        it->size = half_size;

        it->body = new Body(PhysicsUtils::totalMass(c, body), PhysicsUtils::getCenterOfMass(c, body));
        insertBody(it->children[cquad], c, getPosQuad(cquad, top, half_size), half_size);
        insertBody(it->children[bquad], body, getPosQuad(bquad, top, half_size), half_size);
        return;
    }
    it->body->setMass(PhysicsUtils::totalMass(it->body, body));
    it->body->setPos(PhysicsUtils::getCenterOfMass(it->body, body));
    insertBody(it->children[bquad], body, getPosQuad(bquad, top, half_size), half_size);
}



void BarnesHutTree::constructTree() {
    for (auto &x: *(this->bodies)) {
        insertBody(this->root, x, utils::Vector2 (0, 0), this->size);
    }
}

void BarnesHutTree::computeNetForceHelper(utils::Vector2  &sum, barnes_hut_node *it, Body *body) {
    if (it == NULL) {
        return;
    }
    if (it->body == body) {
        return;
    }
    double s = it->size.norm();
    double d = (it->body->getPos() - body->getPos()).norm();
    
    if (s / d < theta) {
        sum += PhysicsUtils::getForce(it->body, body);
        
        return;
    } 
    bool ok = 0;
    for (int i = 0; i < 4; i++) {
        if (it->children[i] != NULL)
            ok = 1;
        computeNetForceHelper(sum, it->children[i], body);
    }
    if (!ok) {
        sum += PhysicsUtils::getForce(it->body, body);
    }
    
}

utils::Vector2  BarnesHutTree::computeNetForce(Body *body) {
    utils::Vector2 force(0, 0);
    computeNetForceHelper(force, this->root, body);
    
    return force;
}

void BarnesHutTree::walk(double dt) {
    this->constructTree();
    for (auto &x : *(this->bodies)) {
        utils::Vector2 force = this->computeNetForce(x);
        x->setAcceleration(PhysicsUtils::getAccelFromForce(x, force));
    }
    for (auto &x: *(this->bodies)) {
        x->updateMovement(dt);
    }
    delete this->root;
    for (auto x = this->bodies->begin(); x != this->bodies->end(); ) {
        if ((*x)->getPos().x < 0 || (*x)->getPos().x > this->size.x || (*x)->getPos().y < 0 || (*x)->getPos().y > this->size.y) {
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