#ifndef GEAROENIX_PHYSICS_BODY_BODY_HPP
#define GEAROENIX_PHYSICS_BODY_BODY_HPP
#include "../../core/asset/cr-asset.hpp"
#include <memory>
namespace gearoenix {
namespace physics {
    class Transferable;
    namespace body {
        class Body : public core::asset::Asset {
        public:
            class Behaviour {
            public:
                typedef enum {
                    STATIC,
                    RIGID,
                    CAR,
                } Type;
            };

        private:
        protected:
            const Behaviour::Type body_type;
            const std::shared_ptr<Transferable> body_obj;
            Body(const core::Id my_id, const Behaviour::Type body_type, const std::shared_ptr<Transferable>& body_obj);

        public:
            virtual ~Body();
            Behaviour::Type get_body_type() const;
            const std::shared_ptr<Transferable>& get_body_obj() const;
        };
    }
}
}
#endif
