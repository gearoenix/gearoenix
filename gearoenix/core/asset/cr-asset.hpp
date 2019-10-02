#ifndef GEAROENIX_CORE_ASSET_ASSET_HPP
#define GEAROENIX_CORE_ASSET_ASSET_HPP
#include "../cr-static.hpp"
#include "../cr-types.hpp"
#include "cr-asset-type.hpp"
namespace gearoenix::core::asset {
class Asset {
private:
protected:
    GX_GET_CVAL_PRT(Id, asset_id)
    GX_GET_CVAL_PRT(Type, asset_type)
    Asset(const Id asset_id, const Type asset_type) noexcept
        : asset_id(asset_id)
        , asset_type(asset_type)
    {
    }

public:
    virtual ~Asset() noexcept = default;
};
}
#endif
