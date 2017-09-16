#ifndef GEAROENIX_CORE_GC_RANGE_HPP
#define GEAROENIX_CORE_GC_RANGE_HPP
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
        class Gc;
        class Object;
        class Range {
            friend class Gc;

        private:
            list::Node<Object*>* start;
            list::Node<Object*>* end;
            Range(list::Node<Object*>* start, list::Node<Object*>* end);

        protected:
        public:
            ~Range();
        };
    }
}
}
#endif // GEAROENIX_CORE_GC_RANGE_HPP
