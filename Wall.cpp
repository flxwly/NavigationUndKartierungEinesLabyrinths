#include "Wall.hpp"


Wall::Wall(float x1, float y1, float x2, float y2) {
    this->m_a = {x1, y1};
    this->m_b = {x2, y2};
}

Wall::Wall(sf::Vector2f a, sf::Vector2f b) {
    this->m_a = a;
    this->m_b = b;
}

sf::Vector2f Wall::getA() const {
    return this->m_a;
}

sf::Vector2f Wall::getB() const {
    return this->m_b;
}
