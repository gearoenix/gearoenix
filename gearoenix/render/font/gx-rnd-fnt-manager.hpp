#pragma once
#include <boost/container/flat_map.hpp>
#include <memory>
#include <string>

namespace gearoenix::render::font {
struct Font;
struct Manager final {
private:
    boost::container::flat_map<std::string, std::shared_ptr<Font>> fonts;

public:
    Manager();
    ~Manager();
    [[nodiscard]] static Manager& get();
    [[nodiscard]] std::shared_ptr<Font> get(const std::string& name);
};
}