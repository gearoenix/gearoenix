#ifndef GEAROENIX_RENDER_LIGHT_CASCADE_INFO_HPP
#define GEAROENIX_RENDER_LIGHT_CASCADE_INFO_HPP
#include "../../math/math-vector.hpp"
#include <array>
#include <vector>

struct FrameCascadeInfo;

namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace model {
    class Model;
}
namespace sync {
    class Semaphore;
}
namespace light {
    /// For shadow rendering following function happen in order:
    ///     - update (in kernels)
    ///     - start  (in main)    [because it begins command buffers]
    ///     - shadow (in kernels)
    ///     - shrink (in main)
    ///     - record (in kernels)
    ///     - submit (in main)
    class CascadeInfo {
    private:
        /// It is now owner of engine
        engine::Engine* const e;
        std::vector<FrameCascadeInfo*> frames;

    public:
        explicit CascadeInfo(engine::Engine* e) noexcept;
        ~CascadeInfo() noexcept;

        void update(const math::Mat4x4& m, const std::vector<std::array<math::Vec3, 4>>& p) noexcept;

        void start() noexcept;

        void shadow(const model::Model*, std::size_t kernel_index) noexcept;
        
		void shrink() noexcept;
        
		void record(std::size_t kernel_index) noexcept;
        
		void submit() noexcept;
    };
}
}
#endif