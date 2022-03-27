#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP

#include <SFML/Graphics.hpp>

class Cell : public sf::RectangleShape {
public:
    static sf::Color traversableColor;
    static sf::Color nonTraversableColor;
    static sf::Color reachableColor;
    static sf::Color nonReachableColor;

    Cell() = default;

    Cell(sf::Vector2f pos, sf::Vector2f size, bool isTraversable);

    void makeTraversable();

    void makeNonTraversable();

    void makeReachable();

    void makeNonReachable();

    void flag();

    void deFlag();

    void check();

    bool isFlagged() const;

    bool isReachable() const;

    bool isTraversable() const;

    bool isChecked() const;

private:

    bool m_flag = false, m_checked = false;
    bool m_isTraversable = false, m_isReachable = false;
};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP
