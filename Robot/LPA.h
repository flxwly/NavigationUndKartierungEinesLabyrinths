#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LPA_H
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LPA_H

#include "LabyrinthMap.hpp"
#include "Queue.h"

class LPA {
public:
    LPA(sf::Vector2f start, sf::Vector2f goal, LabyrinthMap *map);

    static float heuristic(sf::Vector2f p1, sf::Vector2f p2);

    void initialRun();
    void computeShortestPath();
    void updateCell(Cell *cell);

    std::vector<Cell *> getShortestPath();
private:

    Queue queue;

    LabyrinthMap *m_map;
    Cell *m_goal, *m_start;

};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LPA_H
