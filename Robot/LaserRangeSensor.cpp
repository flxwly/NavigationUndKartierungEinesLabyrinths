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
    float record = INFINITY;

    // Der Sensor mit der Position als m_pos und Richtung als m_dir
    const float x2 = this->m_pos.x;
    const float y2 = this->m_pos.y;
    const float x3 = this->m_dir.x;
    const float y3 = this->m_dir.y;

    for (auto wall: walls) {
        // Eine Wand mit Punkt A und Punkt B
        const float x0 = wall.getA().x;
        const float y0 = wall.getA().y;
        const float x1 = wall.getB().x;
        const float y1 = wall.getB().y;

        // Weil nicht durch 0 geteilt werden kann und der Nenner für s und t gleich ist.
        const float den = y3 * (x1 - x0) - x3 * (y1 - y0);
        const float sNom = (x3 * (y0 - y2) - y3 * (x0 - x2));
        const float tNom = ((y1 - y0) * (x2 - x0) - (y2 - y0) * (x1 - x0));

        const bool tCheck = (tNom >= 0 && den > 0) || (tNom <= 0 && den < 0);
        const bool sCheck = ((sNom >= 0 && den > 0) && (sNom <= den)) || ((sNom <= 0 && den < 0) && (sNom >= den));

        if (den == 0 || !sCheck || !tCheck) {
            continue;
        }

        const float t = ((y1 - y0) * (x2 - x0) - (y2 - y0) * (x1 - x0)) / den;
        const float x = x2 + t * x3;
        const float y = y2 + t * y3;
        const float d = (x - x2) * (x - x2) + (y - y2) * (y - y2);
        if (d < record) {
            record = d;
        }

    }
    return std::sqrt(record);
}


