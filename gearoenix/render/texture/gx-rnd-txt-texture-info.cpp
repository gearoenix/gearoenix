#include "gx-rnd-txt-texture-info.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include <boost/functional/hash.hpp>

gearoenix::render::texture::TextureInfo::TextureInfo(
    const TextureFormat format,
    const SamplerInfo& sampler_info,
    const std::size_t width,
    const std::size_t height,
    const std::size_t depth,
    const Type type,
    const bool has_mipmap)
    : format(format)
    , sampler_info(sampler_info)
    , width(width)
    , height(height)
    , depth(depth)
    , type(type)
    , has_mipmap(has_mipmap)
{
    update_hash();
}

gearoenix::render::texture::TextureInfo& gearoenix::render::texture::TextureInfo::set_format(const TextureFormat v)
{
    if (v == format) {
        return *this;
    }
    format = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::TextureInfo& gearoenix::render::texture::TextureInfo::set_sampler_info(const SamplerInfo& v)
{
    if (v == sampler_info) {
        return *this;
    }
    sampler_info = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::TextureInfo& gearoenix::render::texture::TextureInfo::set_width(const std::size_t v)
{
    if (v == width) {
        return *this;
    }
    width = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::TextureInfo& gearoenix::render::texture::TextureInfo::set_height(const std::size_t v)
{
    if (v == height) {
        return *this;
    }
    height = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::TextureInfo& gearoenix::render::texture::TextureInfo::set_depth(const std::size_t v)
{
    if (v == depth) {
        return *this;
    }
    depth = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::TextureInfo& gearoenix::render::texture::TextureInfo::set_type(const Type v)
{
    if (v == type) {
        return *this;
    }
    type = v;
    update_hash();
    return *this;
}

gearoenix::render::texture::TextureInfo& gearoenix::render::texture::TextureInfo::set_has_mipmap(const bool v)
{
    if (v == has_mipmap) {
        return *this;
    }
    has_mipmap = v;
    update_hash();
    return *this;
}

bool gearoenix::render::texture::TextureInfo::operator==(const TextureInfo& o) const
{
    return hash == o.hash && format == o.format && sampler_info == o.sampler_info && width == o.width && height == o.height && depth == o.depth && type == o.type && has_mipmap == o.has_mipmap;
}

void gearoenix::render::texture::TextureInfo::read(platform::stream::Stream& s)
{
    s.read(format);
    sampler_info.read(s);
    s.read(width);
    s.read(height);
    s.read(depth);
    s.read(type);
    s.read(has_mipmap);
    update_hash();
}

void gearoenix::render::texture::TextureInfo::write(platform::stream::Stream& s) const
{
    s.write_fail_debug(format);
    sampler_info.write(s);
    s.write_fail_debug(width);
    s.write_fail_debug(height);
    s.write_fail_debug(depth);
    s.write_fail_debug(type);
    s.write_fail_debug(has_mipmap);
}

void gearoenix::render::texture::TextureInfo::update_hash()
{
    hash = boost::hash_value(typeid(std::remove_cvref_t<std::remove_pointer_t<decltype(this)>>));
    boost::hash_combine(hash, boost::hash_value(format));
    boost::hash_combine(hash, sampler_info.get_hash());
    boost::hash_combine(hash, boost::hash_value(width));
    boost::hash_combine(hash, boost::hash_value(height));
    boost::hash_combine(hash, boost::hash_value(depth));
    boost::hash_combine(hash, boost::hash_value(type));
    boost::hash_combine(hash, boost::hash_value(has_mipmap));
}

std::string std::to_string(const gearoenix::render::texture::TextureInfo& info)
{
    auto result = std::string("TextureInfo {\n");

    result += "    .hash = " + std::to_string(info.get_hash()) + ";\n";
    result += "    .format = " + std::to_string(info.get_format()) + ";\n";
    result += "    .sampler_info = " + std::to_string(info.get_sampler_info()) + ";\n";
    result += "    .width = " + std::to_string(info.get_width()) + ";\n";
    result += "    .height = " + std::to_string(info.get_height()) + ";\n";
    result += "    .depth = " + std::to_string(info.get_depth()) + ";\n";
    result += "    .type = " + std::to_string(info.get_type()) + ";\n";
    result += "    .has_mipmap = " + std::to_string(info.get_has_mipmap()) + ";\n";
    result += "};\n";

    return result;
}

std::string std::to_string(const gearoenix::render::texture::Type t)
{
    switch (t) {
    case gearoenix::render::texture::Type::Texture2D:
        return "Texture2D";
    case gearoenix::render::texture::Type::Texture3D:
        return "Texture3D";
    case gearoenix::render::texture::Type::TextureCube:
        return "TextureCube";
    case gearoenix::render::texture::Type::Target:
        return "Target";
    default:
        return "Unexpected";
    }
}

std::string std::to_string(const gearoenix::render::texture::TextureFormat f)
{
    switch (f) {
    case gearoenix::render::texture::TextureFormat::RgbaFloat16:
        return "RgbaFloat16";
    case gearoenix::render::texture::TextureFormat::RgbFloat16:
        return "RgbFloat16";
    case gearoenix::render::texture::TextureFormat::RgFloat16:
        return "RgFloat16";
    case gearoenix::render::texture::TextureFormat::Float16:
        return "Float16";
    case gearoenix::render::texture::TextureFormat::RgbaFloat24:
        return "RgbaFloat24";
    case gearoenix::render::texture::TextureFormat::RgbFloat24:
        return "RgbFloat24";
    case gearoenix::render::texture::TextureFormat::RgFloat24:
        return "RgFloat24";
    case gearoenix::render::texture::TextureFormat::Float24:
        return "Float24";
    case gearoenix::render::texture::TextureFormat::RgbaFloat32:
        return "RgbaFloat32";
    case gearoenix::render::texture::TextureFormat::RgbFloat32:
        return "RgbFloat32 ";
    case gearoenix::render::texture::TextureFormat::RgFloat32:
        return "RgFloat32 ";
    case gearoenix::render::texture::TextureFormat::Float32:
        return "Float32 ";
    case gearoenix::render::texture::TextureFormat::RgbaUint8:
        return "RgbaUint8 ";
    case gearoenix::render::texture::TextureFormat::D16:
        return "D16";
    case gearoenix::render::texture::TextureFormat::D24:
        return "D24";
    case gearoenix::render::texture::TextureFormat::D32:
        return "D32";
    default:
        return "Unexpected";
    }
}
