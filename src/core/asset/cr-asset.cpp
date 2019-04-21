#include "cr-asset.hpp"
#include "cr-asset-manager.hpp"

gearoenix::core::asset::Asset::Asset(const Id asset_id, const Type::Id asset_type)
    : asset_id(asset_id)
    , asset_type(asset_type)
{
}

gearoenix::core::asset::Asset::~Asset() {}

gearoenix::core::Id gearoenix::core::asset::Asset::get_asset_id() const
{
    return asset_id;
}

gearoenix::core::asset::Type::Id gearoenix::core::asset::Asset::get_asset_type_id() const
{
    return asset_type;
}
