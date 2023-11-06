#include "physics/physics.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex> 

auto now() { return std::chrono::steady_clock::now(); }
 
auto awake_time()
{
    using std::chrono::operator""ms;
    return now() + 5ms;
}
void func(BarnesHutTree *tree) {
    while (1) {
        const auto start{now()};
        std::this_thread::sleep_until(awake_time());
        tree->walk(0.005);
    }
}
int main() {
    sf::Vector2<double> size = sf::Vector2<double>(18000, 18000);

    // create the window
    sf::RenderWindow window(sf::VideoMode(1800, 1000), "My window");

    std::set <Body *> *bodies = new std::set<Body *>;

    sf::RectangleShape shp(sf::Vector2f(5, 5));
    shp.setFillColor(sf::Color::White);

    Body *body = new Body(100000, sf::Vector2<double>(800, 500));
    body->setVelocity(sf::Vector2<double>(0, 0));
    bodies->insert(body);
    body = new Body(1000, sf::Vector2<double>(800, 900));
    body->setVelocity(sf::Vector2<double>(30, 0));
    bodies->insert(body);
    body = new Body(1000, sf::Vector2<double>(800, 100));
    body->setVelocity(sf::Vector2<double>(-30, 0));
    bodies->insert(body);
    body = new Body(1000, sf::Vector2<double>(1200, 500));
    body->setVelocity(sf::Vector2<double>(0, -30));
    bodies->insert(body);
    body = new Body(1000, sf::Vector2<double>(400, 500));
    body->setVelocity(sf::Vector2<double>(0, 30));
    bodies->insert(body);

    sf::Vertex line[2];
    line[0].color  = sf::Color::Red;
    line[1].color = sf::Color::Red;

    window.setFramerateLimit(60);
    // run the program as long as the window is open
    sf::Thread t(&func, new BarnesHutTree(bodies, size, 0.1));
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P) {    
                    t.launch();
                }
                if (event.key.code == sf::Keyboard::N) {
                    sf::Vector2f MousePosition(sf::Mouse::getPosition(window));
                    bodies->insert(new Body(4000, sf::Vector2<double>(MousePosition.x, MousePosition.y)));
                }
                if (event.key.code == sf::Keyboard::M) {
                    sf::Vector2f MousePosition(sf::Mouse::getPosition(window));
                    bodies->insert(new Body(100000, sf::Vector2<double>(MousePosition.x, MousePosition.y)));
                }
                if (event.key.code == sf::Keyboard::Period) {
                    sf::Vector2f MousePosition(sf::Mouse::getPosition(window));
                    bodies->insert(new Body(10000000, sf::Vector2<double>(MousePosition.x, MousePosition.y)));
                }
            }
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);
        for (auto &x: *bodies) {
            sf::Vector2<double> pos = x->getPos();
            shp.setPosition(pos.x, pos.y);
            line[0].position = sf::Vector2f(pos.x, pos.y);
            double b = PhysicsUtils::norm(x->getVelocity());
            sf::Vector2<double> vn = sf::Vector2<double>(x->getVelocity().x/b, x->getVelocity().y/b);
            line[1].position = sf::Vector2f(pos.x + vn.x * 10, pos.y + vn.y * 10);
            window.draw(shp);
            window.draw(line, 2, sf::Lines);
        }
    
        window.display();
    }
    t.wait();
    return 0;
}
