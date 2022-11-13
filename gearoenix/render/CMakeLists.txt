SET(GX_ENGINE_RENDER_SRC_DIR ${GX_ENGINE_SRC_DIR}/render)
SET(GX_ENGINE_RENDER_SRC
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-arc-controller.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-arc-controller.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-builder.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-builder.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-camera.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-camera.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-colour-tuning.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-jet-controller.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-jet-controller.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-projection.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-engine.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-engine.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-specification.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-type.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-type.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-font.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-font.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-builder.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-builder.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-directional.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-directional.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-light.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-light.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-point.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-point.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-id.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-material.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-material.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-pbr.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-pbr.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-transparency.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-unlit.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-unlit.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-mesh.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-mesh.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-builder.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-builder.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-model.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-model.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-baked.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-baked.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-builder.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-builder.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-runtime.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-runtime.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-builder.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-builder.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-scene.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-scene.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-splash.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-splash.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-builder.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-builder.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-skybox.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-skybox.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-attachment.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-attachment.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-binding-point.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-binding-point.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-face.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-face.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-filter.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-format.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-image.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-image.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-pixel-iterator.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-sampler.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-sampler.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-target.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-target.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-2d.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-2d.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-cube.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-cube.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-info.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-info.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-type.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-wrap.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-alignment.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-button.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-button.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-edit.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-edit.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-modal.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-modal.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-text.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-text.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-type.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-widget.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/widget/gx-rnd-wdg-widget.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-build-configuration.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-gltf-loader.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-gltf-loader.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-resolution.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-runtime-configuration.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-runtime-configuration.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-vertex.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-vertex.hpp)
SET(GX_ENGINE_RENDER_SRC ${GX_ENGINE_RENDER_SRC} PARENT_SCOPE)