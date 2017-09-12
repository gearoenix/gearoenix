#include "cr-asset-manager.hpp"
#include "../../render/camera/render-camera.hpp"
#include "../../render/light/render-light.hpp"
#include "../../render/render-engine.hpp"
#include "../../render/scene/render-scene.hpp"
#include "../../render/shader/render-shader.hpp"
#include "../../render/texture/render-texture.hpp"
#include "../../system/sys-app.hpp"
#include "../cache/file/cr-cache-file-sparse.hpp"
#include "../cache/file/cr-cache-file.hpp"
gearoenix::core::asset::Manager::Manager(system::Application* sys_app, const std::string& file)
    : sys_app(sys_app)
    , render_engine(sys_app->render_engine)
    , file(sys_app, file)
    , shaders(file)
    , cameras(file)
    , audios(file)
    , lights(file)
    , textures(file)
    , models(file)
    , scenes(file)
{
}
