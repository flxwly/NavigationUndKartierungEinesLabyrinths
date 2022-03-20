#include "LaserRangeSensor.hpp"

LaserRangeSensor::LaserRangeSensor(float angle, sf::Vector2f offset) : m_constAngle(angle) {
    this->m_offset = offset;
    updateDir(angle);
}

void LaserRangeSensor::updatePos(sf::Vector2f botPos) {
    this->m_pos = botPos + this->m_offset;
}

void LaserRangeSensor::updateDir(float botAngle) {
    const float newAngle = botAngle + this->m_constAngle;
    this->m_dir = {std::cos(newAngle), std::sin(newAngle)};
}

sf::Vector2f LaserRangeSensor::getDir() const {
    return this->m_dir;
}

sf::Vector2f LaserRangeSensor::getPos() const {
    return this->m_pos;
}

float LaserRangeSensor::measureDistance(const std::vector<Wall> &walls) const {

    // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

    float record = INFINITY;

    for (auto wall: walls) {

        // Wand
        const float x1 = wall.getA().x;
        const float y1 = wall.getA().y;
        const float x2 = wall.getB().x;
        const float y2 = wall.getB().y;

        // LRS
        const float x3 = this->m_pos.x;
        const float y3 = this->m_pos.y;
        const float x4 = this->m_dir.x;
        const float y4 = this->m_dir.y;

        const float den = (x1 - x2) * -y4 - (y1 - y2) * -x4;
        if (den == 0) {
            continue;
        }

        const float t = ((x1 - x3) * -y4 - (y1 - y3) * -x4) / den;
        const float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
        if (t > 0 && t < 1 && u >= 0) {
            const float x = x1 + t * (x2 - x1);
            const float y = y1 + t * (y2 - y1);
            const float d = std::pow(x - m_pos.x, 2) + std::pow(y - m_pos.y, 2);
            if (d < record) {
                record = d;
            }
        }
    }

    return std::sqrt(record);
}


