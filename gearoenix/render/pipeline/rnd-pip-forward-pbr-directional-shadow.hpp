#ifndef GEAROEMIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_HPP
#define GEAROEMIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_HPP
#include "rnd-pip-pipeline.hpp"
#include "rnd-pip-resource-set.hpp"
#include "rnd-pip-resource.hpp"
//#include "../../math/math-matrix.hpp"
//#include "../../math/math-vector.hpp"
//#include "../../core/sync/cr-sync-end-caller.hpp"
//#include <vector>
//#include <memory>

namespace gearoenix {
namespace render {
    namespace buffer {
        class Uniform;
        class Manager;
    }
    namespace camera {
        class Camera;
    }
    namespace command {
        class Buffer;
    }
    namespace light {
        class Directional;
    }
    namespace model {
        class Model;
    }
    namespace pipeline {
        class ForwardPbrDirectionalShadowResource : public Resource {
        };
        class ForwardPbrDirectionalShadowResourceSet : public ResourceSet {
        };
        class ForwardPbrDirectionalShadow : public Pipeline {
        };
    }
}
}
#endif