#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LABYRINTHMAP_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LABYRINTHMAP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <queue>
#include <iostream>
#include <list>

#include "Cell.hpp"


class LabyrinthMap : public sf::Drawable {

public:
    LabyrinthMap(sf::Vector2u size, sf::Vector2u cells);

    Cell *getCell(unsigned int x, unsigned int y);

    Cell *getCellAtReal(float x, float y);

    sf::Vector2u getSize() { return m_size; };

    sf::Vector2u getCellCount() { return m_cells; };

    sf::Vector2f getCellSize() { return m_cellSize; };

    std::vector<sf::Vector2f> aStar(sf::Vector2f start, sf::Vector2f end);
    void markNonReachableCells(sf::Vector2f pos);

private:
    sf::Vector2u m_size, m_cells;
    sf::Vector2f m_cellSize;

    std::vector<std::vector<Cell>> m_content;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        for (const auto &row: this->m_content) {
            for (const auto &cell : row) {
                if (!cell.isTraversable() || !cell.isReachable() || cell.isChecked())
                    target.draw(cell, states);
            }
        }
    };
};

#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_LABYRINTHMAP_HPP
