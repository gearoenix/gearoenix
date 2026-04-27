#include "gx-vk-swapchain.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../platform/gx-plt-application.hpp"
#include "../platform/sdl/gx-plt-sdl-application.hpp"
#include "../render/hdr/gx-rnd-hdr-display-peak.hpp"
#include "../render/texture/gx-rnd-txt-manager.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "engine/gx-vk-eng-engine.hpp"
#include "gx-vk-surface.hpp"
#include "image/gx-vk-img-image.hpp"
#include "image/gx-vk-img-view.hpp"
#include "queue/gx-vk-qu-queue.hpp"
#include "sync/gx-vk-sync-semaphore.hpp"

#include <SDL3/SDL_video.h>

#include <utility>

gearoenix::vulkan::Swapchain::Swapchain()
    : Singleton(this)
{
    initialize();
}

gearoenix::vulkan::Swapchain::~Swapchain() = default;

void gearoenix::vulkan::Swapchain::acquire_next_image(const sync::Semaphore& semaphore)
{
    try {
        auto [result, index] = device::Logical::get().get_vulkan_data().acquireNextImageKHR(
            *vulkan_data,
            std::numeric_limits<std::uint64_t>::max(),
            semaphore.get_vulkan_data());
        image_index = index;
        is_valid = true;
    } catch (const vk::OutOfDateKHRError&) {
        is_valid = false;
    } catch (const vk::InitializationFailedError&) {
        is_valid = false;
    } catch (const vk::SystemError& e) {
        GX_LOG_F("Swapchain failed to get the next image: " << e.what());
    }
}

void gearoenix::vulkan::Swapchain::initialize()
{
    frames = { };

    const auto& physical_device = device::Physical::get();
    const auto& logical_device = device::Logical::get();
    const auto& surface = Surface::get();
    const auto caps = physical_device.get_surface_capabilities();
    const auto& formats = physical_device.get_surface_formats();
    vulkan_data = vk::raii::SwapchainKHR { nullptr };

    // Priority list (most expressive first). Each entry maps the surface (format, colourspace)
    // pair to the OETF the shader applies AND the physical headroom the swapchain can transport.
    // `format_max` is independent of OETF: a BGRA8_sRGB swapchain uses OETF=Linear (GPU encodes
    // sRGB on store) but its 8-bit unorm storage caps values at 1.0; an RGBA16F scRGB swapchain
    // uses the same OETF=Linear but float storage carries arbitrary HDR. We need both axes.
    //
    // Each tier lists BGR-order first (Apple/Windows preference) then RGB-order (Android/Linux
    // preference) for the same encoding. The shader path is identical -- only the channel layout
    // in the framebuffer differs, which the driver swizzles to match the source data we write.
    //
    // Tiers, expressive-first:
    //   1) RGBA16F + scRGB linear   -- float HDR; shader writes linear, OS handles range
    //   2) A2*10G10B10 + HDR10 PQ   -- 10-bit HDR; shader writes PQ-encoded
    //   3) A2*10G10B10 + HDR10 HLG  -- 10-bit HLG; shader writes HLG-encoded
    //   4) BGRA8/RGBA8 sRGB         -- GPU does sRGB on store; shader writes linear (8-bit cap)
    //   5) BGRA8/RGBA8 UNORM        -- shader does sRGB OETF manually (8-bit cap)
    struct FormatChoice final {
        vk::Format format;
        vk::ColorSpaceKHR colour_space;
        render::hdr::OutputOetf oetf;
        float format_max; ///< Effective HDR headroom the storage can carry, in SDR-white multiples.
    };
    constexpr std::array priority {
        // Tier 1: float HDR (Apple EDR / Windows scRGB)
        FormatChoice { vk::Format::eR16G16B16A16Sfloat, vk::ColorSpaceKHR::eExtendedSrgbLinearEXT, render::hdr::OutputOetf::Linear, 1024.0f },
        // Tier 2: HDR10 PQ. BGR-order first, then RGB-order (some Android drivers).
        FormatChoice { vk::Format::eA2B10G10R10UnormPack32, vk::ColorSpaceKHR::eHdr10St2084EXT, render::hdr::OutputOetf::Pq, 1024.0f },
        FormatChoice { vk::Format::eA2R10G10B10UnormPack32, vk::ColorSpaceKHR::eHdr10St2084EXT, render::hdr::OutputOetf::Pq, 1024.0f },
        // Tier 3: HDR10 HLG. Signal 1.0 = ~1000 nits peak; per BT.2408 the SDR-white reference is
        // signal 0.75 = ~200 nits. So peak/SDR-white = 5 (not 10) -- driving AgX past peak=5
        // produces values that map to HLG signal > 1.0 and get clipped by the encoding.
        FormatChoice { vk::Format::eA2B10G10R10UnormPack32, vk::ColorSpaceKHR::eHdr10HlgEXT, render::hdr::OutputOetf::Hlg, 5.0f },
        FormatChoice { vk::Format::eA2R10G10B10UnormPack32, vk::ColorSpaceKHR::eHdr10HlgEXT, render::hdr::OutputOetf::Hlg, 5.0f },
        // Tier 4: SDR sRGB-encoded. Apple/Windows usually report BGRA8; modern Android usually RGBA8.
        FormatChoice { vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear, render::hdr::OutputOetf::Linear, 1.0f },
        FormatChoice { vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear, render::hdr::OutputOetf::Linear, 1.0f },
        // Tier 5: SDR UNORM fallback. Manual sRGB OETF in shader.
        FormatChoice { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear, render::hdr::OutputOetf::Srgb, 1.0f },
        FormatChoice { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear, render::hdr::OutputOetf::Srgb, 1.0f },
    };
    auto chosen_index = std::numeric_limits<std::uint32_t>::max();
    auto chosen_format_max = 1.0f;
    for (const auto& choice : priority) {
        for (std::uint32_t i = 0; i < formats.size(); ++i) {
            if (formats[i].format == choice.format && formats[i].colorSpace == choice.colour_space) {
                chosen_index = i;
                active_oetf = choice.oetf;
                chosen_format_max = choice.format_max;
                goto chose;
            }
        }
    }
chose:
    if (chosen_index >= formats.size()) {
        GX_LOG_D("None of the priority swapchain formats are available; falling back to formats[0].");
        chosen_index = 0;
        active_oetf = render::hdr::OutputOetf::Srgb; // assume worst case (UNORM) for the fallback
        chosen_format_max = 1.0f;
    }

    format = formats[chosen_index];

    // Pick AgX peak variant from the actual display + format ceiling. SDL3 covers macOS / iOS /
    // Android / Windows / Linux uniformly; on platforms where the OS won't tell us the headroom
    // (`hdr_enabled && headroom == 1.0`), `closest_agx_peak_index` falls back to format_max.
    auto* const sdl_window = platform::Application::get().get_window();
    active_agx_peak_index = render::hdr::closest_agx_peak_index(render::hdr::query_window_peak(sdl_window), chosen_format_max);

    // SDR white reference nits used for PQ/HLG pre-scaling. SDL3 reports the platform's value
    // when available; 80 nits is the SDR-reference fallback used by Apple / scRGB conventions.
    if (sdl_window) {
        const auto props = SDL_GetWindowProperties(sdl_window);
        if (props != 0) {
            const auto reported = SDL_GetFloatProperty(props, SDL_PROP_WINDOW_SDR_WHITE_LEVEL_FLOAT, 0.0f);
            if (reported > 0.0f) {
                active_sdr_white_nits = reported * 80.0f; // SDL reports a multiplier of 80 nits
            }
        }
    }

    // Linear OETF needs different scaling depending on the swapchain's nit-mapping. scRGB float
    // formats are nit-mapped (Windows: 1.0 = 80 nits absolute; Apple: 1.0 = user SDR-white via the
    // EDR multiplier) so we have to write `engine_linear * sdr_white_nits/80` for engine SDR-white
    // to land at the platform's reported SDR nits. sRGB-encoded BGRA8 swapchains are signal-space
    // (1.0 = display SDR-peak regardless of nits), so the scale is just 1.0.
    if (active_oetf == render::hdr::OutputOetf::Linear && format.format == vk::Format::eR16G16B16A16Sfloat) {
        active_linear_scale = active_sdr_white_nits / 80.0f;
    } else {
        active_linear_scale = 1.0f;
    }

    // Blue-noise dither: half-LSB at the framebuffer's bit depth. 8-bit, 10-bit, and 16-bit float
    // each need different magnitudes; one calibrated value avoids over-dithering 10-bit / float
    // formats (where the engine's per-channel precision is far higher than 8-bit).
    switch (format.format) {
    case vk::Format::eR16G16B16A16Sfloat:
        active_dither_lsb = 0.0f; // float storage has no quantization to break
        break;
    case vk::Format::eA2B10G10R10UnormPack32:
    case vk::Format::eA2R10G10B10UnormPack32:
        active_dither_lsb = 1.0f / 1023.0f; // 10-bit unorm
        break;
    case vk::Format::eB8G8R8A8Srgb:
    case vk::Format::eB8G8R8A8Unorm:
    case vk::Format::eR8G8B8A8Srgb:
    case vk::Format::eR8G8B8A8Unorm:
    default:
        active_dither_lsb = 1.0f / 255.0f; // 8-bit unorm
        break;
    }

    // Publish the active peak so cameras on the AgX preset (with "track swapchain" enabled) pick
    // up HDR-state changes the next frame. Guarded because Swapchain construction happens before
    // texture::Manager during Engine startup; the second initialize() (called from initialize_frame
    // after all managers are alive) does the actual propagation.
    if (render::texture::Manager::singleton_is_valid()) {
        render::texture::Manager::get().set_active_agx_peak_index(active_agx_peak_index);
    }
    GX_ASSERT_D(caps.minImageCount <= frames_in_flight);
    GX_ASSERT_D(caps.maxImageCount == 0 || caps.maxImageCount >= frames_in_flight);

    vk::ImageUsageFlags image_usage = vk::ImageUsageFlagBits::eColorAttachment;
    const auto format_props = physical_device.get_vulkan_data().getFormatProperties(format.format);
    // Enable a transfer source if supported (for screenshots, etc.)
    if ((caps.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferSrc) && (format_props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eTransferSrc)) {
        image_usage |= vk::ImageUsageFlagBits::eTransferSrc;
    }
    // Enable transfer destination if supported (for blitting camera render targets to swapchain)
    if ((caps.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferDst) && (format_props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eTransferDst)) {
        image_usage |= vk::ImageUsageFlagBits::eTransferDst;
    }

    vk::SwapchainCreateInfoKHR info;
    info.surface = surface.get_vulkan_data();
    info.minImageCount = frames_in_flight;
    info.imageFormat = format.format;
    info.imageColorSpace = format.colorSpace;
    if (caps.currentExtent.width != std::numeric_limits<decltype(caps.currentExtent.width)>::max() && caps.currentExtent.width != 0 && caps.currentExtent.height != std::numeric_limits<decltype(caps.currentExtent.height)>::max() && caps.currentExtent.height != 0) {
        info.imageExtent = caps.currentExtent;
    } else {
        const auto window_size = platform::BaseApplication::get().get_window_size();
        info.imageExtent.width = static_cast<decltype(caps.currentExtent.width)>(window_size.x);
        info.imageExtent.height = static_cast<decltype(caps.currentExtent.height)>(window_size.y);
    }
    info.imageUsage = image_usage;
    info.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    info.imageArrayLayers = 1;
    info.imageSharingMode = vk::SharingMode::eExclusive;
    info.presentMode = vk::PresentModeKHR::eFifo;
    info.clipped = true;
    if (caps.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque) {
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    } else if (caps.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) {
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eInherit;
    } else if (caps.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
    } else if (caps.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;
    } else {
        GX_LOG_F("Error composite is unknown.");
    }

    const auto dev = logical_device.get_vulkan_data();
    vulkan_data = vk::raii::SwapchainKHR(logical_device.get_device(), info);
    auto images = dev.getSwapchainImagesKHR(*vulkan_data);
    GX_ASSERT_D(images.size() == frames_in_flight);

    for (std::uint32_t i = 0; i < images.size(); ++i) {
        auto img = std::make_shared<image::Image>(
            "swapchain-img-" + std::to_string(i),
            static_cast<std::uint32_t>(info.imageExtent.width),
            static_cast<std::uint32_t>(info.imageExtent.height),
            static_cast<std::uint32_t>(1),
            vk::ImageType::e2D,
            static_cast<std::uint32_t>(1),
            static_cast<std::uint32_t>(1),
            info.imageFormat,
            vk::ImageCreateFlags { },
            info.imageUsage,
            images[i]);
        img->set_owned(false);
        frames[i].view = std::make_shared<image::View>(std::shared_ptr(img));
        frames[i].present = std::make_unique<sync::Semaphore>("present-" + std::to_string(i));
    }
    is_valid = true;
    image_index = 0;
}

void gearoenix::vulkan::Swapchain::present()
{
    const auto swapchain_handle = *vulkan_data;
    const auto present_semaphore = frames[image_index].present->get_vulkan_data();
    vk::PresentInfoKHR info;
    info.setWaitSemaphores(present_semaphore);
    info.setSwapchains(swapchain_handle);
    info.pImageIndices = &image_index;
    try {
        queue::Queue::get().present(info);
    } catch (const vk::OutOfDateKHRError&) {
        is_valid = false;
    } catch (const vk::SystemError& e) {
        GX_LOG_F("Presentation failed: " << e.what());
    }
}

const gearoenix::vulkan::sync::Semaphore& gearoenix::vulkan::Swapchain::get_present_semaphore() const
{
    return *frames[image_index].present;
}

#endif