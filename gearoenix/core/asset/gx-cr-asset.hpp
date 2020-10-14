#ifndef GEAROENIX_CORE_ASSET_ASSET_HPP
#define GEAROENIX_CORE_ASSET_ASSET_HPP
#include "../gx-cr-static.hpp"
#include "../gx-cr-types.hpp"
#include "gx-cr-asset-type.hpp"
#include <optional>
#include <string>
#include <utility>
namespace gearoenix::core::asset {
class Asset {
    GX_GET_CVAL_PRT(Id, id)
    GX_GET_CVAL_PRT(Type, asset_type)
    GX_GET_REFC_PRT(std::string, name)
protected:
    Asset(const Id id, const Type asset_type, std::string name) noexcept
        : id(id)
        , asset_type(asset_type)
        , name(std::move(name))
    {
    }

public:
    virtual ~Asset() noexcept = default;
};
}
#endif
