#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP

#include <SFML/Graphics.hpp>

class Cell : public sf::RectangleShape {
public:
    static sf::Color traversableColor;
    static sf::Color nonTraversableColor;

    Cell(sf::Vector2f pos, sf::Vector2f size, bool isTraversable);

    void makeTraversable();

    void makeNonTraversable();

    bool isTraversable() const;

private:

    bool m_isTraversable{};
};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP
