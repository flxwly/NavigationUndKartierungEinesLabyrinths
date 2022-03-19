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

    void initLPA(float distToGoal);

    std::vector<Cell *> getPredecessors() const;

    void addPredecessor(Cell *predecessor);

    std::vector<Cell *> getSuccessors() const;

    void addSuccessor(Cell *successor);

    float getK1() const;
    float getK2() const;

    float getG() const;

    void setG(float g);

    float getF() const;

    void setF(float f);

    float getRhs() const;

    void setRhs(float rhs);

    float getH() const;

    void setH(float h);

    bool isMInQueue() const;

    void setMInQueue(bool mInQueue);

    friend bool operator< (const Cell& l, const Cell& r);
    friend bool operator> (const Cell& l, const Cell& r);
    friend bool operator== (const Cell& l, const Cell& r);
    friend bool operator!= (const Cell& l, const Cell& r);
    friend bool operator<= (const Cell& l, const Cell& r);
    friend bool operator>= (const Cell& l, const Cell& r);

private:

    std::vector<Cell *> m_predecessors, m_successors;
    float m_g, m_rhs, m_h, m_f;

    bool m_isTraversable, m_inQueue;
};


#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_CELL_HPP
