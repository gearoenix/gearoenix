SET(GX_ENGINE_RENDER_SRC_DIR ${GX_ENGINE_SRC_DIR}/render)
SET(GX_ENGINE_RENDER_SRC
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-arc-controller.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-arc-controller.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-camera.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-camera.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-jet-controller.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-jet-controller.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-manager.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-manager.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-orthographic.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-orthographic.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-perspective.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-perspective.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-transformation.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-transformation.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-uniform.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/camera/gx-rnd-cmr-uniform.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-engine.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-engine.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-limitations.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-type.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/engine/gx-rnd-eng-type.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-2d.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-2d.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-font.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-font.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-manager.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-manager.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/font/gx-rnd-fnt-type.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gltf/gx-rnd-gltf-loader.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gltf/gx-rnd-gltf-loader.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gltf/gx-rnd-gltf-mesh-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gltf/gx-rnd-gltf-mesh-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gltf/gx-rnd-gltf-node-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gltf/gx-rnd-gltf-node-manager.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-deferred.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-deferred.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-forward-pbr.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-forward-pbr.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-frame-start.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-frame-start.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-g-buffer-filler.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-g-buffer-filler.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-irradiance-convoluter.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-irradiance-convoluter.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-mipmap-generator.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-mipmap-generator.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-node.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-node.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-radiance-convoluter.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-radiance-convoluter.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-shadow-accumulator.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-shadow-accumulator.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-shadow-mapper.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-shadow-mapper.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-skybox-cube.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-skybox-cube.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-skybox-equirectangular.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-skybox-equirectangular.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-ssao.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-ssao.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-type.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-unlit.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/node/gx-rnd-gr-nd-unlit.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/tree/gx-rnd-gr-tr-pbr.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/tree/gx-rnd-gr-tr-pbr.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/tree/gx-rnd-gr-tr-tree.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/graph/tree/gx-rnd-gr-tr-tree.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-cascade-info.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-cascade-info.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-directional.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-directional.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-light.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-light.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-manager.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-manager.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-point.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-point.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/light/gx-rnd-lt-type.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-pbr.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-pbr.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-skybox-cube.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-skybox-cube.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-skybox-equirectangular.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-skybox-equirectangular.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-translucency-mode.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-translucency-mode.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-type.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-unlit.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/material/gx-rnd-mat-unlit.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-builder.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-builder.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-mesh.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/mesh/gx-rnd-msh-mesh.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-dynamic.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-dynamic.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-manager.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-manager.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-mesh.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-mesh.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-model.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-model.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-static.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-static.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-transformation.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-transformation.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/model/gx-rnd-mdl-type.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-baked.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-baked.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-manager.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-manager.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-reflection.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-reflection.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-runtime.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-runtime.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-type.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/reflection/gx-rnd-rfl-type.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-builder.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-builder.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-manager.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-manager.hpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-scene.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-scene.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/scene/gx-rnd-scn-uniform.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-cube.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-cube.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-equirectangular.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-equirectangular.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-manager.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-manager.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-skybox.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-skybox.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-type.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/skybox/gx-rnd-sky-type.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-face.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-face.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-attachment.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-attachment.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-filter.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-format.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-image.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-image.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-manager.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-manager.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-pixel-iterator.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-sampler.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-sampler.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-target.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-2d.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-2d.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-cube.cpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-cube.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-texture-info.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-type.hpp
#        ${GX_ENGINE_RENDER_SRC_DIR}/texture/gx-rnd-txt-wrap.hpp
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
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-runtime-configuration.cpp
        ${GX_ENGINE_RENDER_SRC_DIR}/gx-rnd-runtime-configuration.hpp)
SET(GX_ENGINE_RENDER_SRC ${GX_ENGINE_RENDER_SRC} PARENT_SCOPE)