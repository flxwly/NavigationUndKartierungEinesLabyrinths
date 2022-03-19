#ifndef NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_QUEUE_H
#define NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_QUEUE_H

#include "Cell.hpp"

/// Queue for LPA*
class Queue {
public:
    Queue();

    static int parent(int i) { return (i - 1) / 2; }

    static unsigned int left(unsigned int i) { return (2 * i + 1); }

    static unsigned int right(unsigned int i) { return (2 * i + 2); }

    Cell *top();

    Cell *pop();

    void push(Cell *cell);

    int findCell(Cell *cell, int index = 0);

    void remove(Cell *);

    void removeNoUpdate(Cell *);

    void heapify(unsigned int index);

    bool empty() { return m_content.empty(); };
    bool size() {return m_content.size(); };

private:
    std::vector<Cell *> m_content;


};

#endif //NAVIGATIONUNDKARTIERUNGEINESLABYRINTHS_QUEUE_H
