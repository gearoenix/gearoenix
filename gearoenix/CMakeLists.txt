SET(GX_ENGINE_SRC_DIR "${CMAKE_SOURCE_DIR}/gearoenix")
SET(GX_ENGINE_SRC_DIR ${GX_ENGINE_SRC_DIR} PARENT_SCOPE)

MESSAGE(STATUS "Gearoenix game engine source directory is ${GX_ENGINE_SRC_DIR}.")
MESSAGE(STATUS "Libraries are ${GX_LIBS}.")

#ADD_SUBDIRECTORY(ai)
#ADD_SUBDIRECTORY(audio)
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(dxr)
ADD_SUBDIRECTORY(math)
ADD_SUBDIRECTORY(physics)
ADD_SUBDIRECTORY(render)
ADD_SUBDIRECTORY(platform)
ADD_SUBDIRECTORY(vulkan)

SET(GX_ENGINE_SRC
        ${GX_ENGINE_AI_SRC}
        ${GX_ENGINE_AUDIO_SRC}
        ${GX_ENGINE_CORE_SRC}
        ${GX_ENGINE_DXR_SRC}
        ${GX_ENGINE_MATH_SRC}
        ${GX_ENGINE_PHYSICS_SRC}
        ${GX_ENGINE_PLATFORM_SRC}
        ${GX_ENGINE_RENDER_SRC}
        ${GX_ENGINE_VULKAN_SRC}
        # Dear ImGui files
        ${GX_IMGUI_PATH}/imgui.cpp
        ${GX_IMGUI_PATH}/imgui_demo.cpp
        ${GX_IMGUI_PATH}/imgui_draw.cpp
        ${GX_IMGUI_PATH}/imgui_tables.cpp
        ${GX_IMGUI_PATH}/imgui_widgets.cpp
        ${GX_IMGUI_PATH}/backends/imgui_impl_vulkan.cpp
        ${GX_IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp
        ${GX_IMGUI_FB_PATH}/ImGuiFileDialog.cpp)

SET(GX_ENGINE_TEST_SRC
        ${GX_ENGINE_AI_TEST_SRC}
        ${GX_ENGINE_AUDIO_TEST_SRC}
        ${GX_ENGINE_CORE_TEST_SRC}
        ${GX_ENGINE_MATH_TEST_SRC}
        ${GX_ENGINE_PHYSICS_TEST_SRC}
        ${GX_ENGINE_PLATFORM_TEST_SRC}
        ${GX_ENGINE_RENDER_TEST_SRC}
        ${GX_ENGINE_VULKAN_TEST_SRC}
        PARENT_SCOPE)

# From the cmake docs, "This is intended to set up file tabs in Visual Studio"
IF (MSVC)
	SOURCE_GROUP("ai" FILES ${GX_ENGINE_AI_SRC})
	SOURCE_GROUP("audio" FILES ${GX_ENGINE_AUDIO_SRC})
	SOURCE_GROUP("core" FILES ${GX_ENGINE_CORE_SRC})
	SOURCE_GROUP("dxr" FILES ${GX_ENGINE_DXR_SRC})
	SOURCE_GROUP("math" FILES ${GX_ENGINE_MATH_SRC})
	SOURCE_GROUP("physics" FILES ${GX_ENGINE_PHYSICS_SRC})
	SOURCE_GROUP("platform" FILES ${GX_ENGINE_PLATFORM_SRC})
	SOURCE_GROUP("render" FILES ${GX_ENGINE_RENDER_SRC})
	SOURCE_GROUP("vulkan" FILES ${GX_ENGINE_VULKAN_SRC})
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_PATH}/imgui.cpp")
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_PATH}/imgui_tables.cpp")
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_PATH}/imgui_widgets.cpp")
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_PATH}/imgui_draw.cpp")
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_PATH}/imgui_demo.cpp")
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_PATH}/imgui_widgets.cpp")
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_PATH}/backends/imgui_impl_vulkan.cpp")
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp")
	SOURCE_GROUP("imgui" FILES "${GX_IMGUI_FB_PATH}/ImGuiFileDialog.cpp")
ENDIF()

ADD_LIBRARY(GearoenixGameEngine STATIC ${GX_ENGINE_SRC})

if(GX_HAS_DXR_DEPENDENCY)
	ADD_DEPENDENCIES(GearoenixGameEngine GX_DXR_SHADERS_TARGET)
ENDIF()

FOREACH(GX_LIB ${GX_DEBUG_LIBS})
    TARGET_LINK_LIBRARIES(GearoenixGameEngine debug ${GX_LIB})
ENDFOREACH()

FOREACH(GX_LIB ${GX_RELEASE_LIBS})
    TARGET_LINK_LIBRARIES(GearoenixGameEngine optimized ${GX_LIB})
ENDFOREACH()