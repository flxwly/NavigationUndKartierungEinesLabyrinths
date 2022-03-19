#include "LPA.h"

void LPA::initialRun() {
    for (unsigned int i = 0; i < m_map->getSize().x * m_map->getSize().y; i++) {
        m_map->getCell(i)->initLPA(heuristic(m_map->getCell(i)->getPosition(), m_goal->getPosition()));
    }
    m_start->setRhs(0);
    queue.push(m_start);
}

void LPA::computeShortestPath() {
    while(*queue.top() < *m_goal || m_goal->getRhs() != m_goal->getG()) {
        Cell * cur = queue.top();
        cur->setMInQueue(false);
        queue.pop();

        if (cur->getG() > cur->getRhs()) {
            cur->setG(cur->getRhs());
        } else {
            updateCell(cur);
        }
        for (auto cell: cur->getSuccessors()) {
            updateCell(cell);
        }
    }
}



void LPA::updateCell(Cell *cell) {
    if (cell != m_start) {
        cell->setRhs(INFINITY);
        for (auto predecessor: cell->getPredecessors())
            cell->setRhs(std::min(cell->getRhs(), predecessor->getG() + heuristic(predecessor->getPosition(), cell->getPosition())));

        if (cell->isMInQueue()) {
            cell->setMInQueue(false);
            queue.remove(cell);
        }
        if (cell->getG() != cell->getRhs()) {
            cell->setMInQueue(true);
            queue.push(cell);
        }
    }
}


float LPA::heuristic(sf::Vector2f p1, sf::Vector2f p2) {
    return static_cast<float> (std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2)));
}

LPA::LPA(sf::Vector2f start, sf::Vector2f goal, LabyrinthMap *map) {
    m_start = map->getCellAtReal(start.x, start.y);
    m_goal = map->getCellAtReal(goal.x, goal.y);
    m_map = map;
}

std::vector<Cell *> LPA::getShortestPath() {
    if (m_goal->getG() == INFINITY) {
        return {};
    }

    std::vector<Cell *> path;
    Cell *cur = m_goal;

    return {};
}
