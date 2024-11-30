#pragma once
#include "gx-ai-graph.hpp"
#include <map>
#include <tuple>
#include <utility>
#include <vector>

namespace gearoenix::ai {
struct Dijkstra {
    /// vertex->weight->index
    std::vector<std::map<int, int>> graph;

public:
    /// vertices -> connections -> (index, weight)
    explicit Dijkstra(const std::vector<std::vector<std::tuple<int, int>>>& graph);
    ~Dijkstra();

    Graph<int, int> best_graph(int start) const;
};
}