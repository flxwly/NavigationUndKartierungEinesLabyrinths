#define _USE_MATH_DEFINES

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <chrono>
#include "Robot/Robot.hpp"

bool isInside(Wall wall, Cell *cell) {
    const sf::Vector2f tl = cell->getPosition();
    const sf::Vector2f tr = {tl.x + cell->getSize().x, tl.y};
    const sf::Vector2f bl = {tl.x, tl.y + cell->getSize().y};

    const bool aIsInside = wall.getA().x >= tl.x && tr.x >= wall.getA().x && wall.getA().y >= tl.y && bl.y >= wall.getA().y;
    const bool bIsInside = wall.getB().x >= tl.x && tr.x >= wall.getB().x && wall.getB().y >= tl.y && bl.y >= wall.getB().y;
    return aIsInside || bIsInside;
}

bool isIntersecting(Wall wall, Cell *cell) {
    const sf::Vector2f tl = cell->getPosition();
    const sf::Vector2f tr = {tl.x + cell->getSize().x, tl.y};
    const sf::Vector2f bl = {tl.x, tl.y + cell->getSize().y};
    const sf::Vector2f br = {tr.x, bl.y};

    std::array<std::array<sf::Vector2f, 2>, 4> cellBounds = {
            std::array<sf::Vector2f, 2>{tl, tr},
            std::array<sf::Vector2f, 2>{tr, br},
            std::array<sf::Vector2f, 2>{br, bl},
            std::array<sf::Vector2f, 2>{bl, tl}
    };

    const float x0 = wall.getA().x;
    const float y0 = wall.getA().y;
    const float x1 = wall.getB().x;
    const float y1 = wall.getB().y;

    for (auto bound: cellBounds) {

        const float x2 = bound[0].x;
        const float y2 = bound[0].y;
        const float x3 = bound[1].x;
        const float y3 = bound[1].y;

        // Weil nicht durch 0 geteilt werden kann und der Nenner für s und t gleich ist.
        const float den = (x0 - x1) * (y2 - y3) - (y0 - y1) * (x2 - x3);
        const float sNom = (x0 - x2) * (y2 - y3) - (y0 - y2) * (x2 - x3);
        const float tNom = (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);

        if (den == 0) {
            continue;
        }

        const float t = tNom / den;
        const float s = sNom / den;

        if (0 <= t && t <= 1 && 0 <= s && s <= 1) {
            return true;
        }

//        const bool sCondition = sNom == 0 || (sNom <= den && ((sNom < 0 && den < 0) || (sNom > 0 && den > 0)));
//        const bool tCondition = tNom == 0 || (tNom <= den && ((tNom < 0 && den < 0) || (tNom > 0 && den > 0)));
//
//        if (sCondition && tCondition) {
//            return true;
//        }
    }
    return false;
}

LabyrinthMap createCompleteMap(sf::Vector2u size, sf::Vector2u cells, sf::Vector2f robotPos, Map &map) {

    LabyrinthMap completeMap(size, cells);
    unsigned int cellCount = cells.x * cells.y;


    for (int i = 0; i < cellCount; ++i) {
        Cell *cur = completeMap.getCell(i);
        for (auto wall: map.getWalls()) {
            if (isIntersecting(wall, cur) || isInside(wall, cur)) {
                cur->makeNonTraversable();
                break;
            }
        }
    }

    completeMap.markNonReachableCells(robotPos);
    return completeMap;
}

int main() {

    const sf::Vector2u winSize = {1080, 720};
    const sf::Vector2u cells = {192, 108};
    const unsigned int winWidth = 1080;
    const unsigned int winHeight = 720;

    sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "Navigation und Kartierung eines Labyrinths.");
    //window.setFramerateLimit(1);
    //window.setVerticalSyncEnabled(true);

    Robot robot = Robot({winWidth / 2, winHeight / 2}, winSize, cells, 20, M_PI / 4);

    srand(time(NULL));
    Map map = Map({winWidth, winHeight}, {50, 50}, 1, 3, 0.1);

    // Eine im vorraus berechnete Karte, die alle nicht erreichbaren Felder markiert.
    // Hat ein Roboter alle nicht erreichbaren gefunden kann eine neue Karte generiert werden. Und die gebrauchte Zahl
    // an Zyklen kann gespeichert werden.
    LabyrinthMap completedMap = createCompleteMap(winSize, cells, robot.getPos(), map);

    const int updatesPerFrame = 20;
    auto lastUpdateTime = std::chrono::high_resolution_clock::now();

    long cycles = 0;
    while (window.isOpen()) {
        window.clear({255, 255, 255});

        window.draw(map);
        //window.draw(completedMap);

        for (int i = 0; i < updatesPerFrame; ++i) {
            robot.update(map);
        }

        int score = robot.calculateScore(completedMap);
        if (static_cast<double> (score) / (cells.x * cells.y) > 0.95) {
            std::cout << "Took " << updatesPerFrame * cycles << " cycles...\n";
            return 0;
        }

        window.draw(robot);

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
                robot.setPos(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
            }
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.display();
        cycles++;
    }

    return 0;
}