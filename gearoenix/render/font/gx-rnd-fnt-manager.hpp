#pragma once
#include "../../core/gx-cr-singleton.hpp"
#include <boost/container/flat_map.hpp>
#include <memory>
#include <string>

namespace gearoenix::render::font {
struct Font;
struct Manager final : core::Singleton<Manager> {
private:
    boost::container::flat_map<std::string, std::shared_ptr<Font>> fonts;

public:
    Manager();
    ~Manager() override;
    [[nodiscard]] std::shared_ptr<Font> get_font(const std::string& name);
};
}