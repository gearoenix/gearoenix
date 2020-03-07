#ifndef GEAROENIX_PHYSICS_BODY_BODY_HPP
#define GEAROENIX_PHYSICS_BODY_BODY_HPP
#include "../../core/asset/cr-asset.hpp"
#include <memory>

namespace gearoenix::physics {
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
        Body(core::Id my_id, Behaviour::Type body_type, const std::shared_ptr<Transferable>& body_obj);

    public:
        ~Body() override;
        [[nodiscard]] Behaviour::Type get_body_type() const;
        [[nodiscard]] const std::shared_ptr<Transferable>& get_body_obj() const;
    };
}
}
#endif
