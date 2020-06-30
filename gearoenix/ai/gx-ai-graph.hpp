#ifndef GEAROENIX_AI_GRAPH_HPP
#define GEAROENIX_AI_GRAPH_HPP

#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <vector>

namespace gearoenix::ai {
template <typename VertexData, typename EdgeData>
struct Node {
    std::shared_ptr<VertexData> data;
    std::map<std::shared_ptr<Node<VertexData, EdgeData>>, std::shared_ptr<EdgeData>> connections;

    friend std::ostream& operator<<(std::ostream& os, const Node& n) noexcept
    {
        os << R"({"data":")" << (*(n.data)) << R"(","connections-count":")" << n.connections.size() << "\"}";
        return os;
    }

    void clear() noexcept
    {
        data = nullptr;
        auto cons = std::move(connections);
        for (auto& c : cons) {
            c.second = nullptr;
            c.first->clear();
        }
    }
};

template <typename VertexData, typename EdgeData>
struct Graph {
    std::shared_ptr<Node<VertexData, EdgeData>> head;

    ~Graph() noexcept
    {
        head->clear();
    }

    [[nodiscard]] bool operator==(const Graph& o) const noexcept
    {
        const auto mine = get_all_nodes();
        const auto its = o.get_all_nodes();
        if (mine.size() != its.size())
            return false;
        for (const auto& n : mine) {
            if (its.find(n) == its.end())
                return false;
        }
        return true;
    }

    static void get_all_descendants(
        std::set<std::shared_ptr<Node<VertexData, EdgeData>>>& s,
        const std::shared_ptr<Node<VertexData, EdgeData>>& n) noexcept
    {
        if (s.find(n) != s.end())
            return;
        s.insert(n);
        for (const auto& c : n->connections) {
            get_all_descendants(s, c.first);
        }
    }

    [[nodiscard]] std::set<std::shared_ptr<Node<VertexData, EdgeData>>> get_all_nodes() const noexcept
    {
        std::set<std::shared_ptr<Node<VertexData, EdgeData>>> s;
        get_all_descendants(s, head);
        return s;
    }

    static void print(
        std::ostream& os,
        std::set<std::shared_ptr<Node<VertexData, EdgeData>>>& s,
        const std::shared_ptr<Node<VertexData, EdgeData>>& n) noexcept
    {
        s.insert(n);
        os << "{\"node\":" << (*n) << ",\"connections\":[";
        bool not_first = false;
        for (const auto& c : n->connections) {
            if (s.find(c.first) != s.end())
                continue;
            if (not_first)
                os << ",";
            os << "{\"connection\":";
            print(os, s, c.first);
            os << R"(,"edge_data":")" << *(c.second) << "\"}";
            not_first = true;
        }
        os << "]}";
    }

    friend std::ostream& operator<<(std::ostream& os, const Graph& g) noexcept
    {
        std::set<std::shared_ptr<Node<VertexData, EdgeData>>> s;
        print(os, s, g.head);
        return os;
    }
};
}

#endif