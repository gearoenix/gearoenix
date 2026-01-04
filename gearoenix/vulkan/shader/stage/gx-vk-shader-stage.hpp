#pragma once
#include "../../../core/gx-cr-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../gx-vk-linker.hpp"

#include <vector>

// namespace gearoenix {
// namespace render {
//     struct Engine;
//     namespace device {
//         struct Logical;
//     }
//     namespace shader {
//         namespace stage {
//             struct Stage {
//             private:
//                 VkShaderModule vulkan_data;
//                 device::Logical* device;
//
//             public:
//                 Stage(std::vector<unsigned char>& data, Engine* engine);
//                 ~Stage();
//                 VkShaderModule get_vulkan_data() const;
//             };
//         }
//     }
// }
// }
#endif