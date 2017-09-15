#ifndef GEAROENIX_CORE_GC_OBJECT_HPP
#define GEAROENIX_CORE_GC_OBJECT_HPP
namespace gearoenix {
namespace core {
    namespace gc {
        class Gc;
        class Object {
            friend class Gc;

        private:
            Gc* garbage_collector;

        protected:
            int size;
            int offset;
            int end;

        public:
            Object();
            ~Object();
        };
    }
}
}
#endif // GEAROENIX_CORE_GC_OBJECT_HPP
