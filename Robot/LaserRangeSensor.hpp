#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LASERRANGESENSOR_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LASERRANGESENSOR_HPP

#include <vector>
#include <cmath>
#include "../Wall.hpp"


class LaserRangeSensor {
public:
    LaserRangeSensor(float angle, sf::Vector2f offset);

    float measureDistance(const std::vector<Wall>& walls) const;
    void updatePos(sf::Vector2f botPos);
    void updateDir(float botAngle);

    sf::Vector2f getDir() const;
    sf::Vector2f getPos() const;

private:
    const float m_constAngle;
    sf::Vector2f m_pos, m_offset, m_dir;
};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LASERRANGESENSOR_HPP
