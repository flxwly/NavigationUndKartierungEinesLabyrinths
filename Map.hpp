#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_MAP_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_MAP_HPP


#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "Wall.hpp"

class Map : public sf::Drawable{
public:
    Map(sf::Vector2u size, sf::Vector2u wallCount, unsigned int randomWalls, unsigned int randomBoxes, float wallFrequency);

    std::vector<Wall> getWalls() const;
    sf::Vector2u getSize() const;

private:
    std::vector<Wall> m_content;
    sf::Vector2u m_size;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_MAP_HPP
