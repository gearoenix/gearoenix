#ifndef GEAROENIX_RENDER_FONT_MANAGER_HPP
#define GEAROENIX_RENDER_FONT_MANAGER_HPP
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
    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] std::shared_ptr<Font> get(const std::string& name) noexcept;
};
}
#endif
