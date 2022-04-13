#include "LabyrinthMap.hpp"

LabyrinthMap::LabyrinthMap(sf::Vector2u size, sf::Vector2u cells) {
    m_size = size;
    m_cells = cells;

    m_cellSize = sf::Vector2f(static_cast<float>(size.x) / static_cast<float>(cells.x),
                              static_cast<float>(size.y) / static_cast<float>(cells.y));

    m_content = std::vector<std::vector<Cell>>(cells.x);
    for (int i = 0; i < cells.x; ++i) {
        for (int j = 0; j < cells.y; ++j) {
            m_content.at(i).emplace_back(
                    sf::Vector2f(static_cast<float>(i) * m_cellSize.x, static_cast<float>(j) * m_cellSize.y),
                    m_cellSize, true);
        }
    }
}

Cell *LabyrinthMap::getCell(unsigned int x, unsigned int y) {
    return &m_content.at(x).at(y);
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
    auto heuristic = [](sf::Vector2f a, sf::Vector2f b) {
        return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
    };

    // Eine Node ist ein Tuple mit f-Kosten, vorherige Node;
    auto comp = [](const std::tuple<double, sf::Vector2i> &n1, const std::tuple<double, sf::Vector2i> &n2) {
        return std::get<0>(n1) > std::get<0>(n2);
    };


    // ----------- Initialisierung --------------

    std::array<sf::Vector2i, 8> validSteps = {
            sf::Vector2i(-1, -1), sf::Vector2i(1, 1),
            sf::Vector2i(-1, 1), sf::Vector2i(1, -1),
            sf::Vector2i(1, 0), sf::Vector2i(-1, 0),
            sf::Vector2i(0, 1), sf::Vector2i(0, -1)};

    const sf::Vector2i startIdx = {static_cast<int> (start.x / m_cellSize.x),
                                   static_cast<int> (start.y / m_cellSize.y)};
    const sf::Vector2i endIdx = {static_cast<int> (end.x / m_cellSize.x),
                                 static_cast<int> (end.y / m_cellSize.y)};

    std::priority_queue<std::tuple<double, sf::Vector2i>, std::vector<std::tuple<double, sf::Vector2i>>, decltype(comp)> pq(
            comp);
    std::vector<std::vector<sf::Vector2i>> p(m_cells.x, std::vector<sf::Vector2i>(m_cells.y));
    std::vector<std::vector<double>> d(m_cells.x, std::vector<double>(m_cells.y, INT_MAX));

    d.at(startIdx.x).at(startIdx.y) = 0;
    pq.push(std::make_tuple(heuristic(start, end), startIdx));

    while (!pq.empty()) {

        sf::Vector2i u = std::get<1>(pq.top());
        pq.pop();

        for (auto e: validSteps) {

            // Nachbar Nodes
            sf::Vector2i v = u + e;

            // Ouf of bounds check für eine 1-Dimensionale Reihung, die eine 2-Dimensionale Reigung darstellen soll
            if (v.x < 0 || v.y < 0 || v.x >= m_cells.x || v.y >= m_cells.y) {
                continue;
            }

            if (!getCell(v.x, v.y)->isTraversable()) {
                continue;
            }

            // Bei diagonalen sollen jeweils angrenzende keine Wände sein
            if ((std::abs(e.x) == 1 && std::abs(e.y) == 1) &&
                !(getCell(u.x + e.x, u.y)->isTraversable() && getCell(u.x, e.y + u.y)->isTraversable())) {
                continue;
            }


            if (d.at(v.x).at(v.y) >
                d.at(u.x).at(u.y) + heuristic(getCell(u.x, u.y)->getPosition(), getCell(v.x, v.y)->getPosition())) {
                p.at(v.x).at(v.y) = u;

                d.at(v.x).at(v.y) = d.at(u.x).at(u.y) +
                                    heuristic(getCell(u.x, u.y)->getPosition(), getCell(v.x, v.y)->getPosition());

                pq.push(std::make_tuple(d.at(v.x).at(v.y) + heuristic(getCell(v.x, v.y)->getPosition(),
                                                                      getCell(endIdx.x, endIdx.y)->getPosition()),
                                        v));
            }

            if (v == endIdx) {

                std::vector<sf::Vector2f> path{};
                // Den Pfad zurück verfolgen
                for (sf::Vector2i i = endIdx; i != startIdx;) {
                    path.push_back(getCell(i.x, i.y)->getPosition() + getCell(i.x, i.y)->getSize() / 2.0f);
                    i = p.at(i.x).at(i.y);
                }
                return path;
            }
        }
    }
    // Es wurde kein Pfad gefunden
    return {};
}

void LabyrinthMap::markNonReachableCells(sf::Vector2f pos) {

    // ----------- Initialisierung --------------

    sf::Vector2i startIdx = {static_cast<int>(std::floor(pos.x / m_cellSize.x)),
                             static_cast<int>(std::floor(pos.y / m_cellSize.y))};
    if (startIdx.x < 0 || m_cells.x <= startIdx.x || startIdx.y < 0 || m_cells.y <= startIdx.y) {
        return;
    }

    std::list<sf::Vector2i> queue = {startIdx};

    std::array<sf::Vector2i, 4> validSteps = {sf::Vector2i(-1, 0),
                                              sf::Vector2i(1, 0),
                                              sf::Vector2i(0, 1),
                                              sf::Vector2i(0, -1)};

    while (!queue.empty()) {
        sf::Vector2i u = queue.front();
        queue.pop_front();

        for (auto e: validSteps) {
            sf::Vector2i v = u + e;

            // Ouf of bounds check für eine 1-Dimensionale Reihung, die eine 2-Dimensionale Reigung darstellen soll
            if (v.x < 0 || m_cells.x <= v.x || v.y < 0 || m_cells.y <= v.y) {
                continue;
            }

            if (getCell(v.x, v.y)->isFlagged() || !getCell(v.x, v.y)->isTraversable()) {
                continue;
            }

            getCell(v.x, v.y)->flag();
            queue.push_back(v);
        }
    }

    for (auto &row: m_content) {
        for (auto &cell: row) {
            if (cell.isFlagged()) {
                cell.makeReachable();
                cell.deFlag();
            } else {
                cell.makeNonReachable();
            }
        }
    }
}


