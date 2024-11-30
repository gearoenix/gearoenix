#pragma once
#include "gx-rnd-txt-texture.hpp"

namespace gearoenix::render::texture {
struct Texture2D : Texture {
protected:
    Texture2D(
        std::string name,
        const TextureInfo& info,
        engine::Engine& e);

public:
    ~Texture2D() override;
    [[nodiscard]] virtual void* get_imgui_ptr() const;
};
}