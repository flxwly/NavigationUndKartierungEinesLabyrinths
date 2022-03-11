#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_WALL_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_WALL_HPP


#include <SFML/System/Vector2.hpp>

class Wall {
public:
    Wall(float x1, float y1, float x2, float y2);
    Wall(sf::Vector2f a, sf::Vector2f b);

    sf::Vector2f getA() const;
    sf::Vector2f getB() const;
private:
    sf::Vector2f m_a, m_b;
};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_WALL_HPP
