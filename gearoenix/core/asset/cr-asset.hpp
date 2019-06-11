#ifndef GEAROENIX_CORE_ASSET_ASSET_HPP
#define GEAROENIX_CORE_ASSET_ASSET_HPP
#include "../cr-types.hpp"
#include "cr-asset-type.hpp"
namespace gearoenix::core::asset {
    class Asset {
    private:
    protected:
        const Id asset_id;
        const Type::Id asset_type;
        Asset(const Id asset_id, const Type::Id asset_type) noexcept : asset_id(asset_id), asset_type(asset_type) {}
    public:
        virtual ~Asset() noexcept = default;
        Id get_asset_id() const noexcept { return asset_id; }
        Type::Id get_asset_type_id() const noexcept { return asset_type; }
    };
}
#endif
