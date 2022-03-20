#include <cmath>
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

std::vector<Cell *> Cell::getPredecessors() const {
    return m_predecessors;
}

/// Note: calculate new rhs value after adding predecessor
void Cell::addPredecessor(Cell *predecessor) {
    Cell::m_predecessors.push_back(predecessor);
}

std::vector<Cell *> Cell::getSuccessors() const {
    return m_successors;
}

void Cell::addSuccessor(Cell *successor) {
    Cell::m_successors.push_back(successor);
}

float Cell::getG() const {
    return m_g;
}

void Cell::setG(float g) {
    Cell::m_g = g;
}

float Cell::getRhs() const {
    return m_rhs;
}

void Cell::setRhs(float rhs) {
    Cell::m_rhs = rhs;
}

float Cell::getF() const {
    return m_f;
}

void Cell::setF(float f) {
    Cell::m_f = f;
}

float Cell::getK1() const {
    return std::min(Cell::m_g, Cell::m_rhs) + Cell::m_h;
}

float Cell::getK2() const {
    return std::min(Cell::m_g, Cell::m_rhs);
}

float Cell::getH() const {
    return Cell::m_h;
}

void Cell::setH(float h) {
    Cell::m_h = h;
}

void Cell::initLPA(float distToGoal) {
    setRhs(INFINITY);
    setG(INFINITY);
    setH(distToGoal);
    m_predecessors = {};
    m_successors = {};
}

bool Cell::isMInQueue() const {
    return m_inQueue;
}

void Cell::setMInQueue(bool mInQueue) {
    m_inQueue = mInQueue;
}

bool operator<(const Cell &a, const Cell &b) {
    return a.getK1() < b.getK1() || (a.getK1() == b.getK1() && a.getK2() < b.getK2());
}

bool operator>(const Cell &a, const Cell &b) {
    return a.getK1() > b.getK1() || (a.getK1() == b.getK1() && a.getK2() > b.getK2());
}

bool operator== (const Cell &a, const Cell &b) {
    return a.getK1() == b.getK1() && a.getK2() == b.getK2();
}

bool operator!= (const Cell &a, const Cell &b) {
    return !(a == b);
}

bool operator>= (const Cell &a, const Cell &b) {
    return a == b || a > b;
}

bool operator<= (const Cell &a, const Cell &b) {
    return a == b || a < b;
}

