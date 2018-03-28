#include "cr-asset.hpp"

gearoenix::core::asset::Asset::Asset(const Id asset_id, const AssetType::Type asset_type)
    : asset_id(asset_id)
    , asset_type(asset_type)
{
}

gearoenix::core::asset::Asset::~Asset() {}

gearoenix::core::Id gearoenix::core::asset::Asset::get_asset_id() const
{
    return asset_id;
}

gearoenix::core::asset::Asset::AssetType::Type gearoenix::core::asset::Asset::get_asset_type() const
{
    return asset_type;
}
