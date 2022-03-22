#include <thread>
#include <future>
#include "Robot.hpp"

Robot::Robot(sf::Vector2f pos, sf::Vector2u mapSize, sf::Vector2u cells, unsigned int sensors, float fov) :
        m_map(mapSize, cells) {

    m_pos = pos;
    m_dir = sf::Vector2f(1, 0);

    float angle = -fov / 2;
    while (angle <= fov / 2) {
        m_sensors.push_back({angle, {0, 0}});
        angle += fov / static_cast<float>(sensors);
    }

    // Shape setzen
    const float radius = 5;
    m_shape.setRadius(radius);
    m_shape.setFillColor({0, 0, 255});
    m_shape.setOutlineColor({0, 0, 0});
    m_shape.setPosition(0, 0);
    m_shape.setOrigin({radius / 2, radius / 2});
    m_shape.setPosition(m_pos);

    chooseGoal();
}

sf::Vector2f Robot::getDir() {
    return m_dir;
}

sf::Vector2f Robot::getPos() {
    return m_pos;
}

void Robot::updateSensorPositions() {
    for (auto &lrs: m_sensors) {
        lrs.updatePos(m_pos);
    }
}

void Robot::updateSensorOrientations() {
    for (auto &lrs: m_sensors) {
        lrs.updateDir(std::atan2(m_dir.y, m_dir.x));
    }
}

void Robot::updatePos() {

    if (std::pow(m_pos.x - m_goal.x, 2) + std::pow(m_pos.y - m_goal.y, 2) < 25 ||
        !m_map.getCellAtReal(m_goal.x, m_goal.y)->isReachable()) {
        m_runPathFindingOnUpdate = true;
        chooseGoal();
        return;
    }

    if (!m_path.empty()) {
        unsigned int closestIndex = 0;
        float record = std::pow(m_path.at(0).x - m_pos.x, 2) +
                       std::pow(m_path.at(0).y - m_pos.y, 2);
        for (unsigned int i = 1; i < m_path.size(); i++) {
            const float d = std::pow(m_path.at(i).x - m_pos.x, 2) +
                            std::pow(m_path.at(i).y - m_pos.y, 2);
            if (d < record) {
                closestIndex = i;
                record = d;
            }
        }

        sf::Vector2f dir;
        if (closestIndex == 0) {
            dir = m_path.at(closestIndex) - m_pos;
        } else {
            dir = m_path.at(closestIndex - 1) - m_pos;
        }

        dir /= std::sqrt(dir.x * dir.x + dir.y * dir.y);

        const float dot = std::max(std::min(dir.x * m_dir.x + dir.y * m_dir.y, 1.0f), -1.0f);
        const float angle = std::acos(dot);

        rotate(angle);
        move(0.25);
    } else {
        if (!m_map.getCellAtReal(m_pos.x, m_pos.y)->isTraversable()) {
            move(1);
        }
        m_runPathFindingOnUpdate = true;
    }

    // update sprite
    m_shape.setPosition(m_pos);
}

void Robot::updateMap(const Map &map) {

    std::vector<Cell *> changedCells;
    std::vector<Wall> walls = map.getWalls();

    sf::Vertex line[2];
    for (auto sensor: m_sensors) {
        const float d = sensor.measureDistance(walls);
        if (d < MAX_MEASURE_DIST) {
            const sf::Vector2f collision = sensor.getPos() + sensor.getDir() * d;

            // out of bounds check
            if (collision.x < 0 || collision.x >= map.getSize().x || collision.y < 0 ||
                collision.y >= map.getSize().y) {
                continue;
            }

            Cell *curCell = m_map.getCellAtReal(collision.x, collision.y);

            if (curCell->isTraversable()) {
                curCell->makeNonTraversable();
                changedCells.push_back(curCell);
            }
        }
    }

    if (!changedCells.empty()) {
        m_map.markNonReachableCells(m_pos);
    }

    for (auto changedCell: changedCells) {
        for (auto p: m_path) {
            if (std::abs(changedCell->getPosition().x - p.x) < m_map.getCellSize().x &&
                std::abs(changedCell->getPosition().y - p.y) < m_map.getCellSize().y) {
                m_runPathFindingOnUpdate = true;
                return;
            }
        }
    }
}

void Robot::update(const Map &map) {

    std::future<std::vector<sf::Vector2f>> futurePath;
    if (m_runPathFindingOnUpdate) {
        futurePath = std::async(&LabyrinthMap::aStar, &this->m_map, m_pos, m_goal);
        futurePath.wait();
    }

    updatePos();
    updateMap(map);

    if (futurePath.valid()) {

        futurePath.wait();
        m_path = futurePath.get();
        if (m_path.empty()) {
            chooseGoal();
        } else {
            m_runPathFindingOnUpdate = false;
        }
    }
}


void Robot::chooseGoal() {
    m_goal = {static_cast <float> (rand() * m_map.getSize().x) / static_cast <float> (RAND_MAX),
              static_cast <float> (rand() * m_map.getSize().y) / static_cast <float> (RAND_MAX)};
};

void Robot::move(float factor) {
    auto newPos = m_pos + m_dir * factor;
    if (newPos.x < 0 || newPos.x >= m_map.getCellSize().x * static_cast<float>(m_map.getSize().x) ||
        newPos.y < 0 || newPos.y >= m_map.getCellSize().y * static_cast<float>(m_map.getSize().y)) {
        return;
    }

    if (!m_map.getCellAtReal(newPos.x, newPos.y)->isTraversable()) {
        m_runPathFindingOnUpdate = true;
        return;
    }

    m_pos = newPos;
    updateSensorPositions();
}

void Robot::setPos(sf::Vector2f pos) {
    m_pos = pos;
    m_runPathFindingOnUpdate = true;
    m_map.markNonReachableCells(m_pos);
    updateSensorPositions();
}

void Robot::rotate(float angle) {
    const float newAngle = std::atan2(m_dir.y, m_dir.x) + angle;
    m_dir = {std::cos(newAngle), std::sin(newAngle)};
    updateSensorOrientations();
}

void Robot::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_map);
    target.draw(m_shape);

    sf::Vertex lines[m_sensors.size() * 2];
    for (unsigned int i = 0; i < m_sensors.size(); i += 1) {
        lines[2 * i] = {m_sensors.at(i).getPos(), sf::Color::Red};
        lines[2 * i + 1] = {m_sensors.at(i).getPos() + m_sensors.at(i).getDir() * 5.0f, sf::Color::Green};
    }
    target.draw(lines, m_sensors.size() * 2, sf::Lines);

    sf::Vertex path[m_path.size()];
    for (unsigned int i = 0; i < m_path.size(); i++) {
        path[i] = {m_path.at(i), sf::Color::Green};
    }
    target.draw(path, m_path.size(), sf::LineStrip);
}

int Robot::calculateScore(LabyrinthMap &completeMap) {
    const unsigned int size = std::min(m_map.getCellCount().x * m_map.getCellCount().y,
                                       completeMap.getCellCount().x * completeMap.getCellCount().y);

    int score = 0;
    for (int i = 0; i < size; ++i) {
        if (m_map.getCell(i)->isReachable() == completeMap.getCell(i)->isReachable()) {
            score += 1;
        }
    }

    return score;
}
