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
            std::map<unsigned int, list::List<Range>*> free_sizes;
            std::mutex* lock;
            list::List<GcObject>* objects;

        protected:
        public:
            Gc(int size, Gc* parent);
            Gc(int size);
            ~Gc();
        };
    }
}
}
#endif // GEAROENIX_CORE_GC_GC_HPP
