#include <cmath>
#include "Cell.hpp"

sf::Color Cell::traversableColor = sf::Color(0, 255, 0, 40);
sf::Color Cell::nonTraversableColor = sf::Color(255, 0, 0, 90);

sf::Color Cell::reachableColor = sf::Color(0, 255, 0, 40);
sf::Color Cell::nonReachableColor = sf::Color(255, 0, 0, 90);

Cell::Cell(sf::Vector2f pos, sf::Vector2f size, bool isTraversable) {

    setPosition(pos);
    setSize(size);

    if (isTraversable) {
        makeTraversable();
    } else {
        makeNonTraversable();
    }
    makeReachable();
    m_flag = false;
}

void Cell::makeTraversable() {
    m_isTraversable = true;
    setFillColor(Cell::traversableColor);
}

void Cell::makeNonTraversable() {
    m_isTraversable = false;
    makeNonReachable();
    setFillColor(Cell::nonTraversableColor);
}

void Cell::makeReachable() {
    m_isReachable = true;
    setOutlineColor(Cell::reachableColor);
}

void Cell::makeNonReachable() {
    m_isReachable = false;
    setOutlineColor(Cell::nonReachableColor);
}

bool Cell::isTraversable() const {
    return this->m_isTraversable;
}

bool Cell::isReachable() const {
    return m_isReachable;
}

void Cell::flag() {
    m_flag = true;
}

void Cell::deFlag() {
    m_flag = false;
}

bool Cell::isFlagged() const {
    return m_flag;
}

void Cell::check() {
    m_checked = true;
}

bool Cell::isChecked() const {
    return m_checked;
}
