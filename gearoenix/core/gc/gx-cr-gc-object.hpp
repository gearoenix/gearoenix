#ifndef GEAROENIX_CORE_GC_OBJECT_HPP
#define GEAROENIX_CORE_GC_OBJECT_HPP

namespace gearoenix::core {
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
        explicit Object(unsigned int size);
        virtual ~Object();
        [[nodiscard]] unsigned int get_offset() const;
        [[nodiscard]] unsigned int get_size() const;
        [[nodiscard]] unsigned int get_end() const;
    };
}
}
#endif