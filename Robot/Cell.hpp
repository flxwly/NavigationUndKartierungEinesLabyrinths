#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP

#include <SFML/Graphics.hpp>

class Cell : public sf::RectangleShape {
public:
    static sf::Color traversableColor;
    static sf::Color nonTraversableColor;
    static sf::Color reachableColor;
    static sf::Color nonReachableColor;

    Cell(sf::Vector2f pos, sf::Vector2f size, bool isTraversable);

    void makeTraversable();

    void makeNonTraversable();

    void makeReachable();

    void makeNonReachable();

    void flag();

    void deFlag();

    bool isFlagged() const;

    bool isReachable() const;

    bool isTraversable() const;

private:

    // TODO: Implement flood fill to see what is reachable and what not
    bool m_flag;
    bool m_isTraversable, m_isReachable;
};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP
