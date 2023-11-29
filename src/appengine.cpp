#include "appengine.hpp"

void AppEngine::eventHandler(sf::Event event) {
    if (event.type == sf::Event::Closed) {
        window->close();
        exit(0);
    }

    if (event.type == sf::Event::Resized) {
        // update the view to the new size of the window
        this->view.setSize(event.size.width * window_zoom, event.size.height * window_zoom);
        window->setView(this->view);
    }
   
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            oldMousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            if (!mouseMoving) {
                mouseMoving = true;
            }
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left && mouseMoving) {
            mouseMoving = false;
            sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2i change = oldMousePos - mousePos;
            this->view.move(change.x * window_zoom, change.y * window_zoom);
            this->window->setView(this->view);

        }
    }

    if (event.type == sf::Event::MouseWheelMoved) {
        if (event.mouseWheel.delta > 0) {
            window_zoom *= zoom_per_scroll/1;
            this->view.zoom(zoom_per_scroll/1);
        } else {
            this->view.zoom(1/zoom_per_scroll);
            window_zoom *= 1/zoom_per_scroll;
        }
        // this->view.setCenter(sf::Vector2f(sf::Mouse::getPosition(*window)));
        this->window->setView(view);
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::N) {
            it++;
            if (it == this->bodies->end()) {
                it = this->bodies->begin();
            }
        }
        if (event.key.code == sf::Keyboard::F) {
            isViewFixed = !isViewFixed;
        }
    }
}
void AppEngine::setFramerateLimit(int frames) {
    this->window->setFramerateLimit(frames);
}

void AppEngine::video_loop() {
    while (window->isOpen()) {

        for (auto &x : *bodies) {
            x->updateShape(this->scale);
        }

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window->pollEvent(event)) {
            this->eventHandler(event);
        }
    
        // std::cout << (*it)->getPosition().x << " " << (*it)->getPosition().y << std::endl;
        if (mouseMoving) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            sf::Vector2i change = oldMousePos - mousePos;

            sf::View temp = this->view;
            temp.move(change.x * window_zoom, change.y * window_zoom);
            this->window->setView(temp);
        }
        if (isViewFixed) {
            view.setCenter((*it)->getShape().getPosition());
            window->setView(view);
        }

        // clear the window with black color
        window->clear(sf::Color::Black);

        // printf("%d\n", ++hours_passed);

        for (auto &x : *bodies) {
            window->draw(x->getShape());
        }

        for (auto &x : drawables) {
            window->draw(*x);
        }
        window->display();
    }
}

sf::Thread *AppEngine::loadFromFile(std::string path) {
    std::ifstream fin;
    fin.open(path);
    int count;
    fin >> count;
    // count = 7;
    for (int i = 0; i < count; i++) {
        double mass, rad;
        utils::Vector2 pos, vel;
        int r, g, b;
        fin >> pos.x >> pos.y >> vel.x >> vel.y >> mass >> rad >> r >> g >> b;
        sf::CircleShape shp(rad / this->scale + log10(rad)/M_PI/2 + 5, 100);
        shp.setFillColor(sf::Color(r, g, b));
        Body *sun = new Body(mass, rad, pos, vel, shp);
        
        bodies->push_back(sun);
    }
    it = bodies->begin();
    
    fin.close();

    return new sf::Thread(std::bind(&AppEngine::universe_logic, this, utils::Vector2(1e13, 1e13), 0.5, 6.6743e-11, 0.001, this->scale));
}
void AppEngine::universe_logic(utils::Vector2 size, double theta, double G, double epsilon, double scale) {
    this->scale = scale;
    while(1) {
        BarnesHutTree *bht = new BarnesHutTree(this->bodies, size, theta, G, epsilon);
        bht->walk(this->dt);
        delete bht;
    }
}

sf::Thread *AppEngine::random_bodies() {
    size_t N = 100000, M = 1;
    utils::Vector2 size = {1e10, 1e10};
    utils::Vector2 range = {2 * 1920, 2 * 1080};
    double theta = 0.5;
    double G = 1;
    double epsilon = 0.001;

    srand(time(NULL));
    int ubpx = range.x/2, lbpx = -range.x/2;
    int ubpy = range.y/2, lbpy = -range.y/2;
    int ubv = 0, lbv = -0;
    // bodies->push_back(new Body(100e2, 10, {0, 0}, {0, 0}, sf::CircleShape(10,100)));
    for (int i = 0; i < N * M; i++) {
        utils::Vector2 pos;
        pos.x = (rand() % ((ubpx - lbpx + 1) * 10000)) / 10000.f + lbpx;
        pos.y = (rand() % ((ubpy - lbpy + 1) * 10000)) / 10000.f + lbpy;
        utils::Vector2 vel;
        vel.x = (rand() % (ubv - lbv + 1)) + lbv;
        vel.y = (rand() % (ubv - lbv + 1)) + lbv;
        bool is_big = 0;
        if (rand() % 100 == 0)
            is_big = 0;
        bodies->push_back(new Body(is_big ? 100e2 : 10000, is_big ? 10 : 1, pos, vel, sf::CircleShape(1,100)));
    }
    return new sf::Thread(std::bind(&AppEngine::universe_logic, this, size, theta, G, epsilon, 1));
}

void AppEngine::start() {
    // loadFromFile("planets.txt");
    // size_t N = 50, M = 50;
    // utils::Vector2 size = {1e14, 1e14};
    // double theta = 0.5;
    // double G = 6.67430e-11;
    // double epsilon = 0.001;


    // utils::Vector2 big_vel(1e5, 1e5);
    // double angle = 0;
    // double angle_step = 2 * M_PI / N;
    // double radius = 1e9;
    // double radius_step = 1e8;
    // double vel = 4e3;
    // double vel_step = 1e3;
    // bodies->push_back(new Body(1e29, 1e7, utils::Vector2(0, 0), big_vel, sf::CircleShape(1e7/this->scale, 100)));
    // for (size_t i = 0; i < N; i++) {
    //     angle = 0;
    //     for (size_t j = 0; j < M; j++) {
    //         angle += 2 * M_PI / M;
    //         utils::Vector2 pos;
    //         pos.x = cos(angle) * radius;
    //         pos.y = sin(angle) * radius;

    //         utils::Vector2 velo;
    //         velo.x = cos(angle + M_PI/2) * vel;
    //         velo.y = sin(angle + M_PI/2) * vel;
    //         bodies->push_back(new Body(1e20, 7e6, pos, big_vel + velo, sf::CircleShape(5e6/this->scale, 100)));
    //     }
    //     angle += angle_step;
    //     radius += radius_step;
    //     vel += vel_step;
    // }
    // angle = 0;
    // radius = 1e9;
    // vel = 4e3;

    // utils::Vector2 big_pos(1e10, 1e10);
    // bodies->push_back(new Body(1e30, 1e7, big_pos, -big_vel, sf::CircleShape(1e7/this->scale, 100)));
    // for (size_t i = 0; i < N; i++) {
    //     angle = 0;
    //     for (size_t j = 0; j < M; j++) {
    //         angle += 2 * M_PI / M;
    //         utils::Vector2 pos;
    //         pos.x = cos(angle) * radius;
    //         pos.y = sin(angle) * radius;

    //         utils::Vector2 velo;
    //         velo.x = cos(angle + M_PI/2) * vel;
    //         velo.y = sin(angle + M_PI/2) * vel;
    //         bodies->push_back(new Body(1e20, 7e6, big_pos + pos, -big_vel + velo, sf::CircleShape(5e6/this->scale, 100)));
    //     }
    //     angle += angle_step;
    //     radius += radius_step;
    //     vel += vel_step;
    // }
    // it = bodies->begin();
    sf::Thread *t = loadFromFile("planets.txt");
    t->launch();

    video_loop();
}
