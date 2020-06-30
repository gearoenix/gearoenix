#ifndef GEAROENIX_AI_DIJKSTRA_HPP
#define GEAROENIX_AI_DIJKSTRA_HPP

#include <map>
#include <utility>
#include <vector>

namespace gearoenix::ai {
class Dijkstra {
    std::vector<std::map<int, int>> graph;

public:
    Dijkstra(std::vector<std::map<int, int>> graph) noexcept;
    ~Dijkstra() noexcept;
};
}

#endif