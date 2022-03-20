#include "Queue.h"

Queue::Queue() {

}

Cell *Queue::top() {
    return m_content.front();
}

Cell *Queue::pop() {
    Cell *front = m_content.front();
    std::swap(m_content.front(), m_content.back());
    m_content.pop_back();
    heapify(0);
    return front;
}

void Queue::heapify(unsigned int index) {
    const unsigned int l = left(index);
    const unsigned int r = right(index);
    unsigned int smallest = index;
    if (l < m_content.size() && *m_content[l] < *m_content[smallest])
        smallest = l;
    if (r < m_content.size() && *m_content[r] < *m_content[smallest])
        smallest = r;

    if (smallest != index) {
        std::swap(m_content[index], m_content[smallest]);
        heapify(smallest);
    }
}

void Queue::push(Cell * cell) {
    int i = static_cast<int> (m_content.size());
    m_content.push_back(cell);

    while (i != 0 && *m_content[parent(i)] > *m_content[i]) {
        std::swap(m_content[i], m_content[parent(i)]);
        i = parent(i);
    }
}

int Queue::findCell(Cell *cell, int index) {
    if (m_content.size() >= index)
        return -1;

    if (*m_content[index] == *cell)
        return index;

    if (*m_content[index] < *cell)
        return -1;

    // Suche rechts
    int result = findCell(cell, right(index));
    if (result != -1) return result;

    // Suche links
    return findCell(cell, left(index));

}

void Queue::remove(Cell *cell) {
    int index = findCell(cell);
    if (index == -1)
        return;

    std::swap(m_content[index], m_content.back());
    m_content.pop_back();
    heapify(index);
}

void Queue::removeNoUpdate(Cell* cell) {
    int index = findCell(cell);
    if (index == -1)
        return;

    std::swap(m_content[index], m_content.back());
    m_content.pop_back();
}
