#include <random>
#include <chrono>
#include "Map.hpp"

Map::Map(sf::Vector2u size, sf::Vector2u wallCount, unsigned int randomWalls, unsigned int randomBoxes,
         float wallFrequency) {

    m_size = size;

    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32)};
    std::mt19937_64 rng(ss);
    std::uniform_real_distribution<double> randomNum(0, 1);
    std::uniform_real_distribution<double> randomXCoordinate(0, size.x);
    std::uniform_real_distribution<double> randomYCoordinate(0, size.y);

    const float cellWidth = static_cast<float> (size.x) / static_cast<float> (wallCount.x);
    const float cellHeight = static_cast<float> (size.y) / static_cast<float> (wallCount.y);

    m_content.emplace_back(0, 0, size.x, 0);
    m_content.emplace_back(size.x, 0, size.x, size.y);
    m_content.emplace_back(size.x, size.y, 0, size.y);
    m_content.emplace_back(0, size.y, 0, 0);

    // horizontal walls
    for (unsigned int i = 0; i < wallCount.x; i++) {
        for (unsigned int j = 1; j < wallCount.y; j++) {
            if (randomNum(rng) < wallFrequency) {
                this->m_content.emplace_back(static_cast<float> (i) * cellWidth,
                                             static_cast<float> (j) * cellHeight,
                                             static_cast<float> (i) * cellWidth + cellWidth,
                                             static_cast<float> (j) * cellHeight);
            }
        }
    }

    // vertical walls
    for (unsigned int j = 0; j < wallCount.y; j++) {
        for (unsigned int i = 1; i < wallCount.x; i++) {
            if (randomNum(rng) < wallFrequency) {
                this->m_content.emplace_back(static_cast<float> (i) * cellWidth,
                                             static_cast<float> (j) * cellHeight,
                                             static_cast<float> (i) * cellWidth,
                                             static_cast<float> (j) * cellHeight + cellHeight);
            }
        }
    }

    // random Walls
    for (int i = 0; i < randomWalls; ++i) {
        this->m_content.emplace_back(randomXCoordinate(rng), randomYCoordinate(rng),
                                     randomXCoordinate(rng), randomYCoordinate(rng));
    }

    // random Boxes
    for (int i = 0; i < randomBoxes; ++i) {
        const double x1 = randomXCoordinate(rng);
        const double y1 = randomYCoordinate(rng);

        const double x2 = randomXCoordinate(rng);
        const double y2 = randomYCoordinate(rng);

        m_content.emplace_back(x1, y1, x1, y2);
        m_content.emplace_back(x1, y1, x2, y1);
        m_content.emplace_back(x2, y1, x2, y2);
        m_content.emplace_back(x1, y2, x2, y2);

    }

}

std::vector<Wall> Map::getWalls() const {
    return this->m_content;
}

sf::Vector2u Map::getSize() const {
    return m_size;
}

void Map::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::Vertex line[2];

    for (const auto &wall: m_content) {
        line[0] = sf::Vertex(wall.getA(), sf::Color::Black);
        line[1] = sf::Vertex(wall.getB(), sf::Color::Black);

        target.draw(line, 2, sf::Lines);
    }
}
