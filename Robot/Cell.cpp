#include <cmath>
#include "Cell.hpp"

sf::Color Cell::traversableColor = sf::Color(0, 255, 0, 40);
sf::Color Cell::nonTraversableColor = sf::Color(255, 0, 0, 90);


Cell::Cell(sf::Vector2f pos, sf::Vector2f size, bool isTraversable) {

    setPosition(pos);
    setSize(size);

    if (isTraversable) {
        makeTraversable();
    } else {
        makeNonTraversable();
    }
}

void Cell::makeTraversable() {
    m_isTraversable = true;
    setFillColor(Cell::traversableColor);
}

void Cell::makeNonTraversable() {
    m_isTraversable = false;
    setFillColor(Cell::nonTraversableColor);
}

void Cell::makeReachable() {
    m_isReachable = true;
}

void Cell::makeNonReachable() {
    m_isReachable = false;
}

bool Cell::isTraversable() const {
    return this->m_isTraversable;
}

bool Cell::isReachable() const {
    return m_isReachable;
}