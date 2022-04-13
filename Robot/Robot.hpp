#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_ROBOT_HPP
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_ROBOT_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include <random>
#include "LabyrinthMap.hpp"
#include "LaserRangeSensor.hpp"
#include "../Map.hpp"

#define MAX_MEASURE_DIST INFINITY

class Robot : public sf::Drawable {
public:

    Robot(sf::Vector2f pos, sf::Vector2u mapSize, sf::Vector2u cells, unsigned int sensors, float fov,
          int movementType = 0);

    void move(float factor);

    void rotate(float angle);

    void update(const Map &map);

    sf::Vector2f getPos();

    void setPos(sf::Vector2f pos);

    sf::Vector2f getDir();

    void calculateScore(LabyrinthMap &completeMap);

    std::list<double> getScore() const { return score; };

    int getMovementType() const {return m_movementType; };

private:

    const int m_movementType = 0;
    sf::Vector2f m_pos, m_dir;
    LabyrinthMap m_map;

    std::vector<LaserRangeSensor> m_sensors;
    sf::Vector2f m_goal;
    bool m_runPathFindingOnUpdate = false;
    std::vector<sf::Vector2f> m_path;

    // for circular Goal
    sf::Vector2i curCell;
    sf::Vector2i curSteps, curMaxSteps;
    sf::Vector2i stepIncrease;
    bool positiveDirection;

    // for score
    std::list<double> score{0};

    void updateSensorPositions();

    void updateSensorOrientations();

    void updateMap(const Map &map);

    void updatePos(const Map &map);

    void chooseRandomGoal();

    void chooseSmartGoal();

    void chooseCircularGoal();

    // Behavior
    void randomMovement(const Map &map);

    void randomPathMovement(const Map &map);

    void chosenPathMovement(const Map &map);

    void circularExplorationMovement(const Map &map);


    sf::RectangleShape m_shape;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_ROBOT_HPP
