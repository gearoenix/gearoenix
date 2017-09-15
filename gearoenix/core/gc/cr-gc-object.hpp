#ifndef GEAROENIX_CORE_GC_OBJECT_HPP
#define GEAROENIX_CORE_GC_OBJECT_HPP
namespace gearoenix {
namespace core {
    namespace gc {
        class Gc;
        class GcObject {
            friend class Gc;

        private:
            GcObject(int size);

        protected:
            int size;
            int offset;
            Gc* garbage_collector;

        public:
            GcObject(int size, Gc* garbage_collector);
            ~GcObject();
        };
    }
}
}
#endif // GEAROENIX_CORE_GC_GC_HPP
