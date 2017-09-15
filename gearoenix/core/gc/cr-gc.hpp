#ifndef GEAROENIX_CORE_GC_GC_HPP
#define GEAROENIX_CORE_GC_GC_HPP
#include "cr-gc-object.hpp"
#include <map>
namespace std {
class mutex;
}
namespace gearoenix {
namespace core {
    namespace list {
        template <typename T>
        class List;
        template <typename T>
        class Node;
    }
    namespace gc {
        template <typename T>
        class Range;
        class Gc : public GcObject {
        private:
            std::map<unsigned int, std::map<unsigned int, Range>> free_sizes;
            std::mutex* lock;
            list::List<Object*>* objects;

        protected:
        public:
            Gc(unsigned int size);
            ~Gc();
            void allocate(Object* obj);
        };
    }
}
}
#endif // GEAROENIX_CORE_GC_GC_HPP
