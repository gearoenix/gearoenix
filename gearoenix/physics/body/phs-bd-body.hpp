#ifndef GEAROENIX_PHYSICS_BODY_BODY_HPP
#define GEAROENIX_PHYSICS_BODY_BODY_HPP
#include "../../math/math-vector.hpp"
#include "../phs-transformable.hpp"
namespace gearoenix {
namespace physics {
    namespace body {
        class Body : public Transferable {
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

        public:
            Body(const Behaviour::Type body_type);
            virtual ~Body();
            Behaviour::Type get_type() const;
        };
    }
}
}
#endif
