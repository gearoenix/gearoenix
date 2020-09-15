#include <gearoenix/ai/gx-ai-graph.hpp>
#include <gearoenix/system/gx-sys-log.hpp>
#include <sstream>

BOOST_AUTO_TEST_CASE(gx_ai_graph)
{
    using GxNode = gearoenix::ai::Node<int, int>;
    using GxGraph = gearoenix::ai::Graph<int, int>;

    std::shared_ptr<GxNode> n1(new GxNode());
    std::shared_ptr<GxNode> n2(new GxNode());
    n1->data = std::make_shared<int>(1);
    n1->connections[n2] = std::make_shared<int>(8);
    n2->data = std::make_shared<int>(2);
    n2->connections[n1] = n1->connections[n2];

    GxGraph g1;
    g1.head = n1;

    BOOST_TEST_MESSAGE("graph: " << g1);

    std::stringstream ss;
    ss << g1;

    BOOST_TEST(ss.str() == R"({"node":{"data":"1","connections-count":"1"},"connections":[{"connection":{"node":{"data":"2","connections-count":"1"},"connections":[]},"edge_data":"8"}]})");
}