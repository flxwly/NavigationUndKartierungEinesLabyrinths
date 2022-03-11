#include "Cell.hpp"

sf::Color Cell::traversableColor = sf::Color(0, 255, 0, 90);
sf::Color Cell::nonTraversableColor = sf::Color(255, 0, 0);


Cell::Cell(sf::Vector2f pos, sf::Vector2f size, bool isTraversable) {

    setPosition(pos);
    setSize(size);
//    setOutlineColor({0, 0, 0});
//    setOutlineThickness(1);

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

bool Cell::isTraversable() const {
    return this->m_isTraversable;
}
