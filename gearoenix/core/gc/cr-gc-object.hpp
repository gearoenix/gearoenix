#ifndef GEAROENIX_CORE_GC_OBJECT_HPP
#define GEAROENIX_CORE_GC_OBJECT_HPP
namespace gearoenix {
namespace core {
    namespace list {
        template <class T>
        class Node;
    }
    namespace gc {
        class Gc;
        class Object {
            friend class Gc;

        private:
            Gc* garbage_collector = nullptr;
            list::Node<Object*>* node = nullptr;

        protected:
            unsigned int size;
            unsigned int offset = 0;
            unsigned int end;

        public:
            Object(unsigned int size);
            ~Object();
        };
    }
}
}
#endif // GEAROENIX_CORE_GC_OBJECT_HPP
