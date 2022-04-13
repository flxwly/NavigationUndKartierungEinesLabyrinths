#include <thread>
#include <future>
#include "Robot.hpp"

Robot::Robot(sf::Vector2f pos, sf::Vector2u mapSize, sf::Vector2u cells, unsigned int sensors, float fov,
             int movementType) :
        m_map(mapSize, cells), m_movementType(movementType) {

    m_pos = pos;
    m_dir = sf::Vector2f(1, 0);

    float angle = -fov / 2;
    while (angle <= fov / 2) {
        m_sensors.push_back({angle, {0, 0}});
        angle += fov / static_cast<float>(sensors);
        m_sensors.back().updatePos(pos);
    }

    // Shape setzen
    const sf::Vector2f size = {5, 5};
    m_shape.setSize(size);
    m_shape.setFillColor({0, 0, 255});
    m_shape.setOutlineColor({0, 0, 0});
    m_shape.setPosition(0, 0);
    m_shape.setOrigin(size / 2.0f);
    m_shape.setPosition(m_pos);

    m_goal = m_pos;


    // for circular Goal
    switch (movementType) {
        default:
            stepIncrease = {1, 1};
            break;
        case 4:
            stepIncrease = {4, 4};
            break;
        case 5:
            stepIncrease = {16, 16};
            break;
        case 6:
            stepIncrease = sf::Vector2i(std::floor(m_map.getCellCount().x / 16),
                                        std::floor(m_map.getCellCount().y / 16));
            break;
    }

    curCell = sf::Vector2i(std::floor(pos.x / m_map.getCellSize().x),
                           std::floor(pos.y / m_map.getCellSize().y));
    curMaxSteps = stepIncrease;
    curSteps = sf::Vector2i(0, 0);
    positiveDirection = true;
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

void Robot::updatePos(const Map &map) {

    switch (m_movementType) {
        case 0:
            randomMovement(map);
            break;
        case 1:
            randomPathMovement(map);
            break;
        case 2:
            chosenPathMovement(map);
            break;
        case 3:
        case 4:
        case 5:
        case 6:
            circularExplorationMovement(map);
            break;
        default:
            break;
    };

    // update sprite
    m_shape.setPosition(m_pos);
}

void Robot::updateMap(const Map &map) {

    std::vector<Cell *> changedCells;
    std::vector<Wall> walls = map.getWalls();

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

    updatePos(map);
    updateMap(map);

    if (m_runPathFindingOnUpdate) {
        m_path = m_map.aStar(m_pos, m_goal);
        if (!m_path.empty()) {
            m_runPathFindingOnUpdate = false;
        }
    }
}

void Robot::move(float factor) {
    auto newPos = m_pos + m_dir * factor;
    if (newPos.x < 0 || newPos.x >= m_map.getCellSize().x * static_cast<float>(m_map.getSize().x) ||
        newPos.y < 0 || newPos.y >= m_map.getCellSize().y * static_cast<float>(m_map.getSize().y)) {
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
    m_shape.setRotation(newAngle * 180 / 3.14152);
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

void Robot::calculateScore(LabyrinthMap &completeMap) {
    const sf::Vector2u cells = {std::min(m_map.getCellCount().x, completeMap.getCellCount().x),
                                std::min(m_map.getCellCount().y, completeMap.getCellCount().y)};

    int revealed = 0;
    int existing = 0;
    for (int i = 0; i < cells.x; ++i) {
        for (int j = 0; j < cells.y; ++j) {
            if (!m_map.getCell(i, j)->isReachable()) {
                revealed += 1;
            }
            if (!completeMap.getCell(i, j)->isReachable()) {
                existing += 1;
            }
        }
    }

    score.push_back(static_cast<double>(revealed) / existing);
}

void Robot::randomMovement(const Map &map) {

    std::default_random_engine randomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0, 90);

    auto toUsableAngle = [](int angle) {
        return static_cast <float> (angle - 45) * 180.0f / 3.14152f;
    };

    float lowest = INFINITY;
    for (auto sensor: m_sensors) {
        const float d = sensor.measureDistance(map.getWalls());
        if (lowest > d) {
            lowest = d;
        }
    }

    if (lowest > 0.3) {
        move(0.25);
    }

    if (lowest < 5) {
        rotate(toUsableAngle(distribution(randomEngine)));
    }
}

void Robot::randomPathMovement(const Map &map) {
    if (std::pow(m_pos.x - m_goal.x, 2) + std::pow(m_pos.y - m_goal.y, 2) < 25 ||
        !m_map.getCellAtReal(m_goal.x, m_goal.y)->isReachable()) {
        m_runPathFindingOnUpdate = true;
        chooseRandomGoal();
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
        randomMovement(map);
        m_runPathFindingOnUpdate = true;
    }
}

void Robot::chosenPathMovement(const Map &map) {
    if (std::pow(m_pos.x - m_goal.x, 2) + std::pow(m_pos.y - m_goal.y, 2) < 25 ||
        !m_map.getCellAtReal(m_goal.x, m_goal.y)->isReachable()) {
        m_runPathFindingOnUpdate = true;
        chooseSmartGoal();
        return;
    }
    m_map.getCellAtReal(m_pos.x, m_pos.y)->check();

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
        randomMovement(map);
        m_runPathFindingOnUpdate = true;
    }
}

void Robot::circularExplorationMovement(const Map &map) {
    if (!m_map.getCellAtReal(m_goal.x, m_goal.y)->isReachable() ||
        m_map.getCellAtReal(m_goal.x, m_goal.y)->isChecked()) {
        m_runPathFindingOnUpdate = true;
        chooseCircularGoal();
        return;
    }
    m_map.getCellAtReal(m_pos.x, m_pos.y)->check();

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
        randomMovement(map);
        m_runPathFindingOnUpdate = true;
    }
}

void Robot::chooseRandomGoal() {
    m_goal = {static_cast <float> (rand() * m_map.getSize().x) / static_cast <float> (RAND_MAX),
              static_cast <float> (rand() * m_map.getSize().y) / static_cast <float> (RAND_MAX)};
}

void Robot::chooseSmartGoal() {
    const float radiusOfCell = std::sqrt(m_map.getCellSize().x * m_map.getCellSize().x
                                         + m_map.getCellSize().y * m_map.getCellSize().y);
    sf::Vector2f finalGoal;
    float record = 0;

    for (int i = 1; i < m_map.getCellCount().x - 1; ++i) {
        for (int j = 1; j < m_map.getCellCount().y - 1; ++j) {
            auto cur = m_map.getCell(i, j);
            if (!cur->isTraversable()) {
                const sf::Vector2f directedVector = cur->getPosition() + cur->getSize() / 2.0f - m_pos;
                const float magnitude = std::sqrt(directedVector.x * directedVector.x
                                                  + directedVector.y * directedVector.y);
                const sf::Vector2f newGoal = m_pos + directedVector + radiusOfCell * directedVector / magnitude;
                if (newGoal.x < 0 || newGoal.y < 0 || newGoal.x >= m_map.getSize().x || newGoal.y >= m_map.getSize().y)
                    continue;

                if (magnitude < record) {
                    continue;
                }

                cur = m_map.getCellAtReal(newGoal.x, newGoal.y);
                if (cur->isReachable() && !cur->isChecked()) {
                    record = magnitude;
                    finalGoal = newGoal;
                }
            }
        }
    }
    if (record < INFINITY) {
        m_goal = finalGoal;
        m_map.getCellAtReal(finalGoal.x, finalGoal.y)->check();
    } else {
        chooseRandomGoal();
    }
}

void Robot::chooseCircularGoal() {

    if (curSteps.x < curMaxSteps.x) {
        if (positiveDirection) {
            curCell.x += 1;
        } else {
            curCell.x -= 1;
        }

        curSteps.x += 1;
    } else if (curSteps.y < curMaxSteps.y) {
        if (positiveDirection) {
            curCell.y += 1;
        } else {
            curCell.y -= 1;
        }

        curSteps.y += 1;
    } else {
        curMaxSteps += stepIncrease;
        curSteps.x = 0;
        curSteps.y = 0;
        positiveDirection = !positiveDirection;
        chooseCircularGoal();
        return;
    }

    if (curCell.x < 0 || curCell.x >= m_map.getCellCount().x || curCell.y < 0 || curCell.y >= m_map.getCellCount().y) {
        chooseCircularGoal();
        return;
    }

    if (m_map.getCell(curCell.x, curCell.y)->isChecked() || !m_map.getCell(curCell.x, curCell.y)->isReachable()) {
        chooseCircularGoal();
        return;
    }

    m_goal = sf::Vector2f(static_cast<float>(curCell.x) * m_map.getCellSize().x,
                          static_cast<float>(curCell.y) * m_map.getCellSize().y);
}
