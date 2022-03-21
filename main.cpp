#define _USE_MATH_DEFINES

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <chrono>
#include "Robot/Robot.hpp"

int main() {

    const sf::Vector2u winSize = {1080, 720};
    const sf::Vector2u cells = {192, 108};
    const unsigned int winWidth = 1080;
    const unsigned int winHeight = 720;

    sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "Navigation und Kartierung eines Labyrinths.");
    //window.setFramerateLimit(1);
    //window.setVerticalSyncEnabled(true);

    Robot robot = Robot({winWidth / 2, winHeight / 2}, winSize, cells, 20, M_PI/3);
    Map map = Map({winWidth, winHeight}, {100, 100}, 5, 0.3);
    srand(time(NULL));

    const int updatesPerFrame = 10;

    auto lastUpdateTime = std::chrono::high_resolution_clock::now();

    // run the program as long as the window is open
    while (window.isOpen()) {
        window.clear({255, 255, 255});

        window.draw(map);

        for (int i = 0; i < updatesPerFrame; ++i) {
            robot.update(map);
        }

        window.draw(robot);

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::MouseButtonPressed) {
                robot.setPos(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
            }
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.display();
    }

    return 0;
}
