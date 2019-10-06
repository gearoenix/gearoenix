#ifndef GEAROENIX_CORE_ASSET_ASSET_HPP
#define GEAROENIX_CORE_ASSET_ASSET_HPP
#include "../cr-static.hpp"
#include "../cr-types.hpp"
#include "cr-asset-type.hpp"
#include <optional>
#include <string>
namespace gearoenix::core::asset {
class Asset {
    GX_GET_CVAL_PRT(Id, asset_id)
    GX_GET_CVAL_PRT(Type, asset_type)
    GX_GET_CREF_PRT(std::optional<std::string>, name)
protected:
    Asset(const Id asset_id, const Type asset_type) noexcept
        : asset_id(asset_id)
        , asset_type(asset_type)
    {
    }

    Asset(const Id asset_id, const Type asset_type, std::string name) noexcept
        : asset_id(asset_id)
        , asset_type(asset_type)
        , name(name)
    {
    }

public:
    virtual ~Asset() noexcept = default;
    void set_name(const std::string& n) noexcept { name = n; }
};
}
#endif
