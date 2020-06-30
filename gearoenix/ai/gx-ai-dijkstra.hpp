#ifndef GEAROENIX_AI_DIJKSTRA_HPP
#define GEAROENIX_AI_DIJKSTRA_HPP
#include "gx-ai-graph.hpp"
#include <map>
#include <tuple>
#include <utility>
#include <vector>

namespace gearoenix::ai {
class Dijkstra {
    /// vertex->weight->index
    std::vector<std::map<int, int>> graph;

public:
    /// vertices -> connections -> (index, weight)
    Dijkstra(const std::vector<std::vector<std::tuple<int, int>>>& graph) noexcept;
    ~Dijkstra() noexcept;

    Graph<int, int> best_graph(int start) const noexcept;
};
}

#endif