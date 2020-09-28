#ifndef GEAROENIX_TEST_AI_DIJKSTRA_HPP
#define GEAROENIX_TEST_AI_DIJKSTRA_HPP

#include <gearoenix/ai/gx-ai-dijkstra.hpp>
#include <gearoenix/system/gx-sys-log.hpp>
#include <sstream>

BOOST_AUTO_TEST_CASE(gearoenix_ai_dijkstra_test)
{
    using GxDijkstra = gearoenix::ai::Dijkstra;
    GxDijkstra dij({ { { 1, 4 }, { 7, 8 } },
        { { 2, 8 }, { 0, 4 }, { 7, 11 } },
        { { 1, 8 }, { 8, 2 }, { 3, 7 }, { 5, 4 } },
        { { 2, 7 }, { 5, 14 }, { 4, 9 } },
        { { 3, 9 }, { 5, 10 } },
        { { 4, 10 }, { 3, 14 }, { 2, 4 }, { 6, 2 } },
        { { 5, 2 }, { 8, 6 }, { 7, 1 } },
        { { 8, 7 }, { 1, 11 }, { 0, 8 }, { 6, 1 } },
        { { 2, 2 }, { 6, 6 }, { 7, 7 } } });

    auto g = dij.best_graph(0);
    // this for now
}

#endif