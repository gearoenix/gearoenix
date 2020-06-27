#ifndef GEAROENIX_PHYSICS_BODY_BODY_HPP
#define GEAROENIX_PHYSICS_BODY_BODY_HPP
#include "../../core/asset/cr-asset.hpp"
#include <memory>

namespace gearoenix::physics {
class Transferable;
}

namespace gearoenix::physics::body {
class Body : public core::asset::Asset {
public:
    enum struct Behaviour {
        STATIC,
        RIGID,
        CAR,
    };

private:
protected:
    const Behaviour body_type;
    const std::shared_ptr<Transferable> body_obj;
    Body(core::Id my_id, std::string name, Behaviour body_type, std::shared_ptr<Transferable> body_obj);

public:
    ~Body() override;
    [[nodiscard]] Behaviour get_body_type() const;
    [[nodiscard]] const std::shared_ptr<Transferable>& get_body_obj() const;
};
}
#endif
