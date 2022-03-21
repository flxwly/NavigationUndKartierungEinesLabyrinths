#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_ROBOT_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_ROBOT_HPP

#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <cmath>
#include "LabyrinthMap.hpp"
#include "LaserRangeSensor.hpp"
#include "../Map.hpp"

#define MAX_MEASURE_DIST INFINITY

class Robot : public sf::Drawable {
public:

    Robot(sf::Vector2f pos, sf::Vector2u mapSize, sf::Vector2u cells, unsigned int sensors, float fov);

    void move(float factor);
    void rotate(float angle);

    void update(const Map& map);
    sf::Vector2f getPos();
    void setPos(sf::Vector2f pos);
    sf::Vector2f getDir();

private:

    sf::Vector2f m_pos, m_dir;
    LabyrinthMap m_map;
    std::vector<LaserRangeSensor> m_sensors;

    sf::Vector2f m_goal;
    bool m_runPathFindingOnUpdate = true;
    std::vector<sf::Vector2f> m_path;

    void updateSensorPositions();
    void updateSensorOrientations();
    void updateMap(const Map& map);
    void updatePos();

    void chooseGoal();

    sf::CircleShape m_shape;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_ROBOT_HPP
