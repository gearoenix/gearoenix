#include "gx-ai-dijkstra.hpp"
#include <limits>
#include <optional>

gearoenix::ai::Dijkstra::Dijkstra(const std::vector<std::vector<std::tuple<int, int>>>& g)
{
    graph.reserve(g.size());
    for (auto& c : g) {
        std::map<int, int> v;
        for (auto [i, w] : c) {
            v[w] = i;
        }
        graph.emplace_back(std::move(v));
    }
}

gearoenix::ai::Graph<int, int> gearoenix::ai::Dijkstra::best_graph(const int start) const
{
    struct SeenInfo {
        int weight = std::numeric_limits<int>::max();
        bool not_best = true;
    };
    std::vector<SeenInfo> seen_vertices;
    seen_vertices.resize(graph.size(), SeenInfo {});
    seen_vertices[start].weight = 0;
    int idx = start;
    for (std::size_t i = 0; i < graph.size(); ++i) {
        int min_w = std::numeric_limits<int>::max();
        for (std::size_t j = 0; j < graph.size(); ++j) {
            const auto& v = seen_vertices[j];
            if (v.not_best && v.weight < min_w) {
                min_w = v.weight;
                idx = static_cast<int>(j);
            }
        }
        seen_vertices[idx].not_best = false;
        const auto& connections = graph[idx];
        for (const auto [cw, ci] : connections) {
            auto& c = seen_vertices[ci];
            const int new_w = cw + min_w;
            if (c.weight > new_w && c.not_best) {
                c.weight = new_w;
            }
        }
    }
    return gearoenix::ai::Graph<int, int>();
}

gearoenix::ai::Dijkstra::~Dijkstra() = default;
