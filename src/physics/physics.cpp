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
    return (bod1->getMass() * bod1->getPos() + bod2->getMass() * bod2->getPos()) / (bod1->getMass() + bod2->getMass());
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
    
    sf::Vector2<double> force(1, 1);
    double rn = PhysicsUtils::norm(r);
    force *= PhysicsUtils::G * bod1->getMass() * bod2->getMass() / (rn * rn);

    r /= PhysicsUtils::norm(r);
    
    force = PhysicsUtils::dotProduct(force, r);
    return -force;
}

size_t chooseQuad(sf::Vector2<double> pos, sf::Vector2<double> top, sf::Vector2<double> size) {
    sf::Vector2<double> mid = top + PhysicsUtils::dotProduct(size, sf::Vector2<double>(0.5, 0.5));
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
sf::Vector2<double> getPosQuad(size_t quad, sf::Vector2<double> top, sf::Vector2<double> half_size) {
    if (quad == 0) {
        return top;
    }
    if (quad == 1) {
        return sf::Vector2<double>(top.x + half_size.x, top.y);
    }
    if (quad == 2) {
        return sf::Vector2<double>(top.x, top.y + half_size.y);
    }
    return sf::Vector2<double>(top.x + half_size.x, top.y + half_size.y);
}
void BarnesHutTree::insertBody(barnes_hut_node* &it, Body *body, sf::Vector2 <double> top, sf::Vector2 <double> size) {
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
    sf::Vector2<double> half_size = PhysicsUtils::dotProduct(size, sf::Vector2<double>(0.5, 0.5));
    


    size_t bquad = chooseQuad(body->getPos(), top, size);
    // External node
    if (std::find(this->bodies->begin(), this->bodies->end(), it->body) != this->bodies->end()) {
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
        insertBody(this->root, x, sf::Vector2<double>(0, 0), this->size);
    }
}

void BarnesHutTree::computeNetForceHelper(sf::Vector2<double> &sum, barnes_hut_node *it, Body *body) {
    if (it == NULL) {
        return;
    }
    if (it->body == body) {
        return;
    }
    double s = PhysicsUtils::norm(it->size);
    double d = PhysicsUtils::norm(it->body->getPos() - body->getPos());
    
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

sf::Vector2<double> BarnesHutTree::computeNetForce(Body *body) {
    sf::Vector2<double> force(0, 0);
    computeNetForceHelper(force, this->root, body);
    
    return force;
}

void BarnesHutTree::walk(double dt) {
    this->constructTree();
    for (auto &x : *(this->bodies)) {
        sf::Vector2<double> force = this->computeNetForce(x);
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