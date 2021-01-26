FUNCTION(GX_PREPARE_IMGUI IMGUI_PATH)
    # Vulkan backend
    FILE(READ ${IMGUI_PATH}/backends/imgui_impl_vulkan.h IMGUI_FILE)
    STRING(CONCAT REPLACE_STR
            "#ifndef GEAROENIX_VULKAN_IMGUI_HPP\n"
            "#define GEAROENIX_VULKAN_IMGUI_HPP\n"
            "#include \"../render/gx-rnd-build-configuration.hpp\"\n"
            "#ifdef GX_RENDER_VULKAN_ENABLED\n"
            "#include \"../platform/macro/gx-plt-mcr-disable-warnings.hpp\"\n"
            "\n")
    STRING(REPLACE "#pragma once" ${REPLACE_STR} IMGUI_FILE "${IMGUI_FILE}")
    STRING(REPLACE "#include \"imgui.h\"" "#include <imgui.h>" IMGUI_FILE "${IMGUI_FILE}")
    STRING(REPLACE "#include <vulkan/vulkan.h>" "#include \"gx-vk-loader.hpp\"" IMGUI_FILE "${IMGUI_FILE}")
    SET(DES_FILE_PATH "${CMAKE_SOURCE_DIR}/gearoenix/vulkan/gx-vk-imgui.hpp")
    FILE(WRITE ${DES_FILE_PATH} "${IMGUI_FILE}")
    FILE(APPEND ${DES_FILE_PATH} "#include \"../platform/macro/gx-plt-mcr-enable-warnings.hpp\"\n#endif\n#endif")

    FILE(READ ${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp IMGUI_FILE)
    STRING(CONCAT REPLACE_STR
            "#include \"gx-vk-imgui.hpp\"\n"
            "#ifdef GX_RENDER_VULKAN_ENABLED\n"
            "#include \"../platform/macro/gx-plt-mcr-disable-warnings.hpp\"")
    STRING(REPLACE "#include \"imgui.h\"" ${REPLACE_STR} IMGUI_FILE "${IMGUI_FILE}")
    STRING(REPLACE "#include \"imgui_impl_vulkan.h\"" "" IMGUI_FILE "${IMGUI_FILE}")
    STRING(REPLACE " vk" " gearoenix::vulkan::Loader::vk" IMGUI_FILE "${IMGUI_FILE}")
    SET(DES_FILE_PATH "${CMAKE_SOURCE_DIR}/gearoenix/vulkan/gx-vk-imgui.cpp")
    FILE(WRITE ${DES_FILE_PATH} "${IMGUI_FILE}")
    FILE(APPEND ${DES_FILE_PATH} "\n#include \"../platform/macro/gx-plt-mcr-enable-warnings.hpp\"\n#endif\n")
ENDFUNCTION(GX_PREPARE_IMGUI)