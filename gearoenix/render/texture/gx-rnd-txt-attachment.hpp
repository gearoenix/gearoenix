#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-txt-face.hpp"
#include <memory>
#include <variant>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct TextureCube;
struct Texture2D;

struct Attachment2D final {
    std::shared_ptr<Texture2D> txt;

    explicit Attachment2D(std::shared_ptr<Texture2D>&& txt);
    ~Attachment2D();
    void write(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const;
    static void read(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<Attachment2D>&& e);
};

struct AttachmentCube final {
    Face face = Face::PositiveZ;
    std::shared_ptr<TextureCube> txt;

    AttachmentCube(std::shared_ptr<TextureCube>&& txt, Face face);
    ~AttachmentCube();
    void write(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const;
    static void read(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<AttachmentCube>&& e);
};

struct Attachment final {
    constexpr static std::uint32_t ATTACHMENT_2D_VARIANT_INDEX = 0;
    constexpr static std::uint32_t ATTACHMENT_CUBE_VARIANT_INDEX = 1;

    std::uint8_t mipmap_level = 0;
    std::variant<Attachment2D, AttachmentCube> var;

    explicit Attachment(const std::variant<Attachment2D, AttachmentCube>& var = Attachment2D { nullptr }, std::uint8_t mipmap_level = 0);
    ~Attachment();
    void write(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const;
    static void read(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<Attachment>&& end);
    static void read(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<std::vector<Attachment>>&& end);
};
}