#ifndef GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_RESOURCE_SET_HPP
#include "rnd-pip-resource-set.hpp"
namespace gearoenix {
namespace render {
    namespace pipeline {
        class ForwardPbrDirectionalShadowResourceSet : public ResourceSet {
        protected:
            std::shared_ptr<texture::Cube> diffuse_environment = nullptr;
            std::shared_ptr<texture::Cube> specular_environment = nullptr;
            std::shared_ptr<texture::Texture2D> ambient_occlusion = nullptr;
            std::shared_ptr<texture::Texture2D> shadow_map = nullptr;
            std::shared_ptr<texture::Texture2D> brdflut = nullptr;

        public:
            virtual void set_node_uniform_buffer(buffer::Uniform* uniform_buffer);
            virtual void set_diffuse_environment(const std::shared_ptr<texture::Cube>& t);
            virtual void set_specular_environment(const std::shared_ptr<texture::Cube>& t);
            virtual void set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t);
            virtual void set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t);
            virtual void set_brdflut(const std::shared_ptr<texture::Texture2D>& t);
            virtual void clean();
        };
    }
}
}
#endif
