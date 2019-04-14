#ifndef GEAROENIX_CORE_ASSET_ASSET_HPP
#define GEAROENIX_CORE_ASSET_ASSET_HPP
#include "../cr-types.hpp"
#include "cr-asset-type.hpp"
namespace gearoenix {
namespace core {
    namespace asset {
        class Asset {
        private:
        protected:
            const Id asset_id;
            const Type::Id asset_type;
            Asset(const Id asset_id, const Type::Id asset_type);

        public:
            virtual ~Asset();
            Id get_asset_id() const;
            Type::Id get_asset_type_id() const;
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix
#endif // GEAROENIX_CORE_ASSET_ASSET_HPP
