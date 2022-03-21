#include "Map.hpp"

Map::Map(sf::Vector2u size, sf::Vector2u wallCount, unsigned int randomWalls, float wallFrequency) {

    m_size = size;

    const float cellWidth = static_cast<float> (size.x) / static_cast<float> (wallCount.x);
    const float cellHeight = static_cast<float> (size.y) / static_cast<float> (wallCount.y);

    srand(time(0));

    // horizontal walls
    for (unsigned int i = 0; i < wallCount.x; i++) {
        for (unsigned int j = 0; j <= wallCount.y; j++) {
            if (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) < wallFrequency || j == 0 ||
                j == wallCount.y) {
                this->m_content.emplace_back(static_cast<float> (i) * cellWidth,
                                             static_cast<float> (j) * cellHeight,
                                             static_cast<float> (i) * cellWidth + cellWidth,
                                             static_cast<float> (j) * cellHeight);
            }
        }
    }

    // vertical walls
    for (unsigned int j = 0; j < wallCount.y; j++) {
        for (unsigned int i = 0; i <= wallCount.x; i++) {
            if (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) < wallFrequency || i == 0 ||
                i == wallCount.x) {
                this->m_content.emplace_back(static_cast<float> (i) * cellWidth,
                                             static_cast<float> (j) * cellHeight,
                                             static_cast<float> (i) * cellWidth,
                                             static_cast<float> (j) * cellHeight + cellHeight);
            }
        }
    }

    // random Walls
    for (int i = 0; i < randomWalls; ++i) {

        const float x1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * size.x;
        const float y1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * size.y;

        const float x2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * size.x;
        const float y2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * size.y;

        this->m_content.emplace_back(x1, y1, x2, y2);
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
};
