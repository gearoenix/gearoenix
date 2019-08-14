#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-pool.hpp"
#include "../core/cr-types.hpp"
#include "../math/math-vector.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <tuple>
#include <vector>
namespace gearoenix {
namespace core::sync {
    class KernelWorker;
}
namespace render {
    namespace camera {
        class Camera;
    }
    namespace light {
        class Directional;
        class CascadeInfo;
        class Light;
    }
    namespace model {
        class Model;
    }
    namespace scene {
        class Scene;
    }
}
namespace system {
    class Application;
}
namespace physics {
    namespace animation {
        class Animation;
    }
    class Engine {
    public:
        template <class T, class S>
        using PairedPool = core::OneLoopPool<std::pair<T, S>>;
        template <class... Types>
        using TupledPool = core::OneLoopPool<std::tuple<Types...>>;
        using ScenePtr = render::scene::Scene*;
        using CascadeInfoPtr = render::light::CascadeInfo*;
        using DirLightPtr = render::light::Directional*;
        using LightPtr = render::light::Light*;
        using CameraPtr = render::camera::Camera*;
        using ModelPtr = render::model::Model*;
        template <class T>
        using ScenePairedPool = PairedPool<ScenePtr, T>;
        template <class T>
        using CameraPairedPool = PairedPool<CameraPtr, T>;
        template <class... Types>
        using CameraTupledPool = TupledPool<CameraPtr, Types...>;
        template <class T>
        using DirLightPairedPool = PairedPool<DirLightPtr, T>;
        template <class T>
        using SceneCameraPairedPool = ScenePairedPool<CameraPairedPool<T>>;
        template <class... Types>
        using SceneCameraTupledPool = ScenePairedPool<CameraTupledPool<Types...>>;
        template <class T>
        using SceneCameraDirLightPairedPool = SceneCameraPairedPool<DirLightPairedPool<T>>;
        using ModelPtrs = std::vector<ModelPtr>;
        using DirLightCascadeInfos = std::vector<std::pair<DirLightPtr, CascadeInfoPtr>>;
        using SceneCameraData = SceneCameraTupledPool<ModelPtrs, DirLightPairedPool<CascadeInfoPtr>>;
        /// Because it's gonna be prioritized in future, it is better to keep its map-ness
        using GatheredSceneCameraData = std::map<ScenePtr, std::map<CameraPtr, std::pair<ModelPtrs, DirLightCascadeInfos>>>;
        using GatheredCascadeInfos = std::map<ScenePtr, std::map<CameraPtr, std::map<DirLightPtr, CascadeInfoPtr>>>;
        using Partition = std::array<math::Vec3, 4>;
        using Partitions = std::vector<Partition>;
        using SceneCameraPartitions = SceneCameraPairedPool<Partitions>;
        using SceneCameraDirLightCascadeInfo = SceneCameraDirLightPairedPool<CascadeInfoPtr>;

    private:
        system::Application* const sys_app;
        core::sync::KernelWorker* const kernels;
        /// if animation return true on its apply its gonna be deleted
        std::map<core::Id, std::shared_ptr<animation::Animation>> animations;
        /// visibility checker
        std::vector<SceneCameraData> kernels_scene_camera_data;
        GatheredSceneCameraData scenes_camera_data;

        std::mutex added_animations_locker;
        std::vector<std::shared_ptr<animation::Animation>> added_animations;
        std::mutex removed_animations_locker;
        std::vector<core::Id> removed_animations;

        /// It does followings:
        ///    - Uniform buffer update for scene.
        ///    - Uniform buffer update for camera.
        ///    - It does the model visibility check.
        ///    - Uniform buffer update for model.
        ///    - Update the shadow caster lights.
        ///    - Compute cameras cascaded shadowing partitions
        void update_001_kernel(unsigned int kernel_index) noexcept;
        void update_001_receiver() noexcept;

        /// It does followings:
        ///    - Directional shadow caster lights cascade kernels gathering
        void update_002_kernel(unsigned int kernel_index) noexcept;
        void update_002_receiver() noexcept;

    public:
        Engine(system::Application* sysapp, core::sync::KernelWorker* kernels) noexcept;
        void add_animation(const std::shared_ptr<animation::Animation>& a) noexcept;
        void remove_animation(const std::shared_ptr<animation::Animation>& a) noexcept;
        void remove_animation(core::Id a) noexcept;
        void update() noexcept;
        const GatheredSceneCameraData& get_visible_models() const noexcept;
        GatheredSceneCameraData& get_visible_models() noexcept;
    };
}
}
#endif
