#include "gx-ai-dijkstra.hpp"

gearoenix::ai::Dijkstra::Dijkstra(std::vector<std::map<int, int>> graph) noexcept
    : graph(std::move(graph))
{
}

gearoenix::ai::Dijkstra::~Dijkstra() noexcept = default;
