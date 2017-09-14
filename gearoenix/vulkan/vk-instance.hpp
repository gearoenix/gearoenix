#ifndef GEAROENIX_VULKAN_INSTANCE_HPP
#define GEAROENIX_VULKAN_INSTANCE_HPP
#include "../core/cr-build-configuration.hpp"
#include "vk-linker.hpp"
#include <memory>
#include <set>
#ifdef DEBUG_MODE
#define VULKAN_INSTANCE_DEBUG
#endif
namespace gearoenix {
namespace render {
    class Instance {
    private:
        Linker* linker;
        VkInstance vulkan_data;
#ifdef VULKAN_INSTANCE_DEBUG
        VkDebugReportCallbackEXT report_callback;
#endif
    public:
        Instance(Linker* l);
        ~Instance();
        const Linker* get_linker() const;
        Linker* get_linker();
        const VkInstance& get_vulkan_data() const;
    };
}
}
#endif
