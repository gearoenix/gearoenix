#ifndef GEAROENIX_RENDER_SCENE_MANAGER_HPP
#define GEAROENIX_RENDER_SCENE_MANAGER_HPP
#include <map>
#include <memory>
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    namespace scene {
        class Scene;
        class Engine;
        class Manager {
        private:
            Engine* engine;
            std::shared_ptr<system::File> asset;
            std::map<uint16_t, std::weak_ptr<Scene>> cached;
            std::vector<uint32_t> offsets;

        public:
            Manager(Engine* engine, const std::shared_ptr<system::File>& asset);
            ~Manager();
            std::shared_ptr<Scene>& get_scene(uint16_t id);
        };
    } // namespace scene
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_SCENE_MANAGER_HPP
