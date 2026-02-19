#include "gx-plt-stm-path.hpp"
#include "../gx-plt-log.hpp"

#include <boost/mp11/algorithm.hpp>

namespace {
constexpr auto absolute_index = boost::mp11::mp_find<decltype(gearoenix::platform::stream::Path::data), gearoenix::platform::stream::AbsolutePath>::value;
constexpr auto asset_index = boost::mp11::mp_find<decltype(gearoenix::platform::stream::Path::data), gearoenix::platform::stream::AssetPath>::value;
}

gearoenix::platform::stream::AbsolutePath::AbsolutePath(std::string&& data)
    : data(std::move(data))
{
}

gearoenix::platform::stream::AssetPath::AssetPath(std::string&& data)
    : data(std::move(data))
{
}

gearoenix::platform::stream::Path::Path(Path&&) noexcept = default;

gearoenix::platform::stream::Path::Path(const Path&) = default;

gearoenix::platform::stream::Path& gearoenix::platform::stream::Path::operator=(Path&&) noexcept = default;

gearoenix::platform::stream::Path& gearoenix::platform::stream::Path::operator=(const Path&) = default;

gearoenix::platform::stream::Path::Path(AbsolutePath&& data)
    : data(std::move(data))
{
}

gearoenix::platform::stream::Path::Path(AssetPath&& data)
    : data(std::move(data))
{
}

gearoenix::platform::stream::Path gearoenix::platform::stream::Path::create_absolute(std::string&& data) { return Path(AbsolutePath(std::move(data))); }

gearoenix::platform::stream::Path gearoenix::platform::stream::Path::create_asset(std::string&& data) { return Path(AssetPath(std::move(data))); }

bool gearoenix::platform::stream::Path::is_absolute() const { return absolute_index == data.index(); }

bool gearoenix::platform::stream::Path::is_asset() const { return asset_index == data.index(); }

const std::string& gearoenix::platform::stream::Path::get_raw_data() const
{
    switch (data.index()) {
    case absolute_index:
        return std::get<absolute_index>(data).data;
    case asset_index:
        return std::get<asset_index>(data).data;
    default:
        GX_UNEXPECTED;
    }
}
