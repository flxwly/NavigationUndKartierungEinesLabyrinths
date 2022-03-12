#include "LabyrinthMap.hpp"

LabyrinthMap::LabyrinthMap(sf::Vector2u size, sf::Vector2u cells) {
    m_size = size;
    m_cells = cells;

    m_cellSize = sf::Vector2f(static_cast<float>(size.x) / static_cast<float>(cells.x),
                              static_cast<float>(size.y) / static_cast<float>(cells.y));

    for (int j = 0; j < cells.y; ++j) {
        for (int i = 0; i < cells.x; ++i) {
            m_content.emplace_back(
                    sf::Vector2f(static_cast<float>(i) * m_cellSize.x, static_cast<float>(j) * m_cellSize.y),
                    m_cellSize, true);
        }
    }

}

Cell *LabyrinthMap::getCell(unsigned int index) {
    return &m_content.at(index);
}

Cell *LabyrinthMap::getCell(unsigned int x, unsigned int y) {
    return getCell(x + y * m_cells.x);
}

Cell *LabyrinthMap::getCellAtReal(float x, float y) {
    return getCell(std::floor(x / m_cellSize.x), std::floor(y / m_cellSize.y));
}

std::vector<sf::Vector2f> LabyrinthMap::aStar(sf::Vector2f start, sf::Vector2f end) {

    // ---------- Seltene Fälle ----------
    if (start.x < 0 || start.x >= m_size.x || start.y < 0 || start.y >= m_size.y ||
        end.x < 0 || end.x >= m_size.x || end.y < 0 || end.y >= m_size.y) {

        return {};
    }

    if (!getCellAtReal(start.x, start.y)->isTraversable()
        || !getCellAtReal(end.x, end.y)->isTraversable()) {

        return {};
    }

    if (start == end) {
        return {start};
    }

    // --------- wichtige lambda Funktionen ------------
    auto idx = [](sf::Vector2u p, unsigned int w) {
        return p.x + p.y * w;
    };

    auto heuristic = [](sf::Vector2f a, sf::Vector2f b) {
        return static_cast<float>(std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2)));
    };

    // Eine Node ist ein Tuple mit f-Kosten, vorherige Node;
    auto comp = [](const std::tuple<float, int> &n1, const std::tuple<float, int> &n2) {
        return std::get<0>(n1) > std::get<0>(n2);
    };


    // ----------- Initialisierung --------------
    const unsigned int size = m_cells.x * m_cells.y;
    const int width = static_cast<int> (m_cells.x);

    std::vector<int> validSteps = {
            width - 1, width, width + 1,
            -1, 1,
            -width - 1, -width, -width + 1};

    const unsigned int startIdx = idx({static_cast<unsigned int> (start.x / m_cellSize.x),
                                       static_cast<unsigned int> (start.y / m_cellSize.y)},
                                      width);
    const unsigned int endIdx = idx({static_cast<unsigned int> (end.x / m_cellSize.x),
                                     static_cast<unsigned int> (end.y / m_cellSize.y)},
                                    width);

    std::priority_queue<std::tuple<float, int>, std::vector<std::tuple<float, int>>, decltype(comp)> pq(comp);
    std::vector<int> p(size); // prev
    std::vector<float> d(size, INT_MAX); // heuristic (filled with max distance)

    d[startIdx] = 0;
    pq.push(std::make_tuple(heuristic(start, end), startIdx));

    while (!pq.empty()) {

        int u = std::get<1>(pq.top());
        pq.pop();

        for (auto e: validSteps) {

            // Nachbar Nodes
            int v = u + e;

            // Ouf of bounds check für eine 1-Dimensionale Reihung, die eine 2-Dimensionale Reigung darstellen soll
            if (((e == 1 || e == -width + 1 || e == width + 1) && v % width == 0)
                || ((e == -1 || e == -width - 1 || e == width - 1) && u % width == 0)) {
                continue;
            }

            if (v < 0 || size <= v) {
                continue;
            }

            // Bei diagonalen sollen jeweils angrenzende keine Wände sein
            if (std::abs(e + width) == 1 && !getCell(u - width)->isTraversable()) {
                continue;
            } else if (std::abs(e - width) == 1 && !getCell(u + width)->isTraversable()) {
                continue;
            }
            if (std::abs(e + 1) == width && !getCell(u - 1)->isTraversable()) {
                continue;
            } else if (std::abs(e - 1) == width && !getCell(u + 1)->isTraversable()) {
                continue;
            }


            if (d.at(v) > d.at(u) + heuristic(getCell(u)->getPosition(), getCell(v)->getPosition()) &&
                getCell(v)->isTraversable()) {
                p.at(v) = u;
                d.at(v) = d.at(u) + heuristic(getCell(u)->getPosition(), getCell(v)->getPosition());

                if (v == endIdx) {

                    std::vector<sf::Vector2f> path{};

                    // Den Pfad zurück verfolgen
                    for (unsigned int i = endIdx; i != startIdx;) {
                        path.push_back(getCell(i)->getPosition() + getCell(i)->getSize() / 2.0f);
                        i = p[i];
                    }
                    return path;
                }

                pq.push(std::make_tuple(d[v] + heuristic(getCell(v)->getPosition(), getCell(endIdx)->getPosition()),
                                        v));
            }
        }
    }
    // Es wurde kein Pfad gefunden
    return {};
    throw std::invalid_argument("Es gibt keinen Pfad vom Start zum Endpunkt");
}


