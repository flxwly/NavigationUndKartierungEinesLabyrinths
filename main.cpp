#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <future>
#include <fstream>
#include "Robot/Robot.hpp"

bool isInside(Wall wall, Cell *cell) {
    const sf::Vector2f tl = cell->getPosition();
    const sf::Vector2f tr = {tl.x + cell->getSize().x, tl.y};
    const sf::Vector2f bl = {tl.x, tl.y + cell->getSize().y};

    const bool aIsInside =
            wall.getA().x >= tl.x && tr.x >= wall.getA().x && wall.getA().y >= tl.y && bl.y >= wall.getA().y;
    const bool bIsInside =
            wall.getB().x >= tl.x && tr.x >= wall.getB().x && wall.getB().y >= tl.y && bl.y >= wall.getB().y;
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

        const bool tCheck = ((tNom >= 0 && den > 0) && (tNom <= den)) || ((tNom <= 0 && den < 0) && (tNom >= den));
        const bool sCheck = ((sNom >= 0 && den > 0) && (sNom <= den)) || ((sNom <= 0 && den < 0) && (sNom >= den));

        if (den != 0 && tCheck && sCheck) {
            return true;
        }
    }
    return false;
}

LabyrinthMap createCompleteMap(sf::Vector2u size, sf::Vector2u cells, sf::Vector2f robotPos, Map &map) {

    LabyrinthMap completeMap(size, cells);

    for (int i = 0; i < cells.x; ++i) {
        for (int j = 0; j < cells.y; ++j) {
            Cell *cur = completeMap.getCell(i, j);
            for (auto wall: map.getWalls()) {
                if (isIntersecting(wall, cur) || isInside(wall, cur)) {
                    cur->makeNonTraversable();
                    break;
                }
            }
        }
    }

    completeMap.markNonReachableCells(robotPos);
    return completeMap;
}

void
runVisible(Map &map, std::vector<Robot> &robots, LabyrinthMap &completeMap, int scoreInterval) {

    sf::RenderWindow window(sf::VideoMode(map.getSize().x, map.getSize().y),
                            "Kartierung und Untersuchung eines Labyrinths.");
    window.setVerticalSyncEnabled(true);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        throw std::invalid_argument("Font konnte nicht geladen werden");
    }
    sf::Text text;
    text.setFont(font);
    text.setPosition(10, 10);
    text.setCharacterSize(16);
    text.setString("Es wird Roboter 1 gezeigt.");
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Red);

    sf::Text score;
    score.setFont(font);
    score.setPosition(10, 30);
    score.setCharacterSize(16);
    score.setString("Score: 0");
    score.setStyle(sf::Text::Bold);
    score.setFillColor(sf::Color::Red);


    int cycles = 0;
    int cyclesPerFrame = 10;
    unsigned int robotToDraw = 0;

    while (window.isOpen()) {
        for (int i = 0; i < cyclesPerFrame; ++i) {
            std::for_each(robots.begin(), robots.end(), [&map](auto& item) {
                item.update(map);
            });

            if (cycles % scoreInterval == 0) {
                std::for_each(robots.begin(), robots.end(), [&completeMap](auto& item) {
                    item.calculateScore(completeMap);
                });
            }

            cycles += 1;
        }
        window.clear({255, 255, 255});
        window.draw(map);
        window.draw(robots.at(robotToDraw));
        score.setString("Score: " + std::to_string(robots.at(robotToDraw).getScore().back()));
        window.draw(text);

        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
                robotToDraw = (robotToDraw + 1) % robots.size();
                text.setString("Es wird Roboter " + std::to_string(robotToDraw + 1) + " gezeigt.");
            }
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.display();
    }
    window.close();
}

void runSim(Map &map, std::vector<Robot> &robots, LabyrinthMap &completeMap, int scoreInterval) {
    int cycles = 0;
    const int maxCycles = 10000;
    while (true) {

        std::for_each(robots.begin(), robots.end(), [&map](auto& item) {
            item.update(map);
        });

        if (cycles % scoreInterval == 0) {
            std::for_each(robots.begin(), robots.end(), [&completeMap](auto& item) {
                item.calculateScore(completeMap);
            });
        }

        switch (cycles) {
            case maxCycles / 5:
                std::cout << "Done 20%...\n";
                break;
            case maxCycles / 5 * 2:
                std::cout << "Done 40%...\n";
                break;
            case maxCycles / 5 * 3:
                std::cout << "Done 60%...\n";
                break;
            case maxCycles / 5 * 4:
                std::cout << "Done 80%...\n";
                break;
            default:
                if (cycles >= maxCycles) {
                    return;
                }
                break;
        }

        cycles++;
    }
}

std::string getMovementTypeName(int movementType) {
    switch (movementType) {
        case 0:
            return "Zufällige Bewegung";
        case 1:
            return "Zufälliges Ziel";
        case 2:
            return "Eigener Algorithmus";
        case 3:
            return "Zirkuläre Bewegung ohne Abstand";
        case 4:
            return "Zirkuläre Bewegung mit 4 Zellen Abstand";
        case 5:
            return "Zirkuläre Bewegung mit 16 Zellen Abstand";
        case 6:
            return "Zirkuläre Bewegung mit angepasstem Abstand";
        default:
            return "Undefiniertes Bewegungsmuster";

    }
}

std::vector<std::pair<std::string, std::list<double>>> runSimulationA() {

    const sf::Vector2u mapSize = {1080, 720};
    const sf::Vector2u cells = {108, 72};
    const sf::Vector2f startPos = sf::Vector2f(mapSize.x / 2, mapSize.y / 2);

    const sf::Vector2u gridWalls = {35, 35};
    const float gridWallFrequency = 0.3;
    const unsigned int randomWalls = 1;
    const unsigned int randomBoxes = 1;

    Map map = Map(mapSize, gridWalls, randomWalls, randomBoxes, gridWallFrequency);
    std::vector<Robot> robots = {
            Robot(startPos, mapSize, cells, 12, 3.14152 / 2, 1),
            Robot(startPos, mapSize, cells, 12, 3.14152 / 2, 2),
            Robot(startPos, mapSize, cells, 12, 3.14152 / 2, 3),
            Robot(startPos, mapSize, cells, 12, 3.14152 / 2, 4),
            Robot(startPos, mapSize, cells, 12, 3.14152 / 2, 5),
            Robot(startPos, mapSize, cells, 12, 3.14152 / 2, 6),
    };


    LabyrinthMap completedMap = createCompleteMap(mapSize, cells, startPos, map);

    runVisible(map, robots, completedMap, 100);

    std::vector<std::pair<std::string, std::list<double>>> score = {

    };
    score.reserve(robots.size());
    for (const auto& bot: robots) {
        score.emplace_back(getMovementTypeName(bot.getMovementType()), bot.getScore());
    }
    return score;
}

int main() {
    srand(100);

    const int threads = 1;
    const int runTimes = 1;

    std::vector<std::vector<std::pair<std::string, std::list<double>>>> globalScores;
    std::vector<std::future<std::vector<std::pair<std::string, std::list<double>>>>> runs;
    runs.reserve(std::min(threads, runTimes));
    for (int i = 0; i < std::min(threads, runTimes); ++i) {
        runs.emplace_back(std::async(&runSimulationA));
    }

    int completedRuns = 0;
    while (runTimes > completedRuns) {
        for (auto &run: runs) {
            if (run.valid()) {
                std::cout << "Completed run!\n";
                globalScores.push_back(std::move(run.get()));
                completedRuns += 1;
                std::swap(run, runs.back());
                runs.pop_back();
                if (completedRuns + runs.size() < runTimes) {
                    runs.push_back(std::async(&runSimulationA));
                }
            }
        }
    }

    std::ofstream outFile("result.txt");
    for (const auto &sim : globalScores) {
        outFile << "Zyklen\t";
        for (int i = 0; i < sim.at(0).second.size() - 1; ++i) {
            outFile << std::to_string(i * 100) << "\t";
        }
        outFile << "\n";
        for (const auto &run: sim) {
            outFile << run.first << "\t";
            for (const auto &score: run.second) {
                outFile << score << "\t";
            }
            outFile << "\n";
        }
        outFile << "\n";
    }
}

