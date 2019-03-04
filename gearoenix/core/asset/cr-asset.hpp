// #ifndef GEAROENIX_CORE_ASSET_ASSET_HPP
// #define GEAROENIX_CORE_ASSET_ASSET_HPP
// #include "../cr-types.hpp"
// namespace gearoenix {
// namespace core {
//     namespace asset {
//         class Asset {
//         public:
//             class AssetType {
//             public:
//                 typedef enum {
//                     AUDIO,
//                     BODY,
//                     CAMERA,
//                     CONSTRAINT,
//                     FONT,
//                     LIGHT,
//                     MESH,
//                     MODEL,
//                     SCENE,
//                     SHADER,
//                     SKYBOX,
//                     TEXTURE,
//                 } Type;
//             };

//         private:
//         protected:
//             const Id asset_id;
//             const AssetType::Type asset_type;
//             Asset(const Id asset_id, const AssetType::Type asset_type);

//         public:
//             virtual ~Asset();
//             Id get_asset_id() const;
//             AssetType::Type get_asset_type() const;
//         };
//     } // namespace asset
// } // namespace core
// } // namespace gearoenix
// #endif // GEAROENIX_CORE_ASSET_ASSET_HPP
