#pragma once
#include <boost/container/flat_map.hpp>
#include <memory>
#include <string>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::font {
struct Font;
struct Manager final {
    engine::Engine& e;

private:
    boost::container::flat_map<std::string, std::shared_ptr<Font>> fonts;

public:
    explicit Manager(engine::Engine& e);
    ~Manager();
    [[nodiscard]] std::shared_ptr<Font> get(const std::string& name);
};
}