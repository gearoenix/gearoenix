#ifndef GEAROENIX_CORE_LIST_NODE_HPP
#define GEAROENIX_CORE_LIST_NODE_HPP
namespace gearoenix {
namespace core {
    namespace list {
        template <class T>
        class List<T>;
        template <class T>
        class Node {
            friend class List<T>;

        private:
            T value;
            List<T>* root = nullptr;
            Node<T>* previous = nullptr;
            Node<T>* next = nullptr;
            Node(
                const T& valuse,
                List<T>* root = nullptr,
                Node<T>* previous = nullptr,
                Node<T>* next = nullptr);

        protected:
        public:
            ~Node();
            Node<T>* get_previous();
            const Node<T>* get_previous() const;
            Node<T>* get_next();
            const Node<T>* get_next() const;
            List<T>* get_root();
            const List<T>* get_root() const;
            T& get_value();
            const T& get_value() const;
            void add_as_previous(const T& previous_value);
            void add_as_next(const T& next_value);
        };
    }
}
}

template <class T>
gearoenix::core::list::Node::Node(
    const T& valuse, List<T>* root, Node<T>* previous, Node<T>* next)
    : value(value)
{
}

template <class T>
gearoenix::core::list::Node::~Node()
{
    if (previous != nullptr) {
        previous->next = next;
    } else {
        root->front = next;
    }
    if (next != nullptr) {
        next->previous = previous;
    } else {
        root->end = previous;
    }
}

template <class T>
gearoenix::core::list::Node* gearoenix::core::list::Node::get_previous()
{
    return previous;
}

template <class T>
const gearoenix::core::list::Node* gearoenix::core::list::Node::get_previous() const
{
    return previous;
}

template <class T>
gearoenix::core::list::Node* gearoenix::core::list::Node::get_next()
{
    return next;
}

template <class T>
const gearoenix::core::list::Node* get_next() const
{
    return next;
}

template <class T>
gearoenix::core::list::List* gearoenix::core::list::Node::get_root()
{
    return root;
}

template <class T>
const gearoenix::core::list::List* gearoenix::core::list::Node::get_root() const
{
    return root;
}

template <class T>
T& gearoenix::core::list::Node::get_value()
{
    return value;
}

template <class T>
const T& gearoenix::core::list::Node::get_value() const
{
    return value;
}

template <class T>
void gearoenix::core::list::Node::add_as_previous(const T& previous_value)
{
    Node<T>* new_node = new Node(previous_value);
    new_node->next = this;
    new_node->previous = previous;
    new_node->root = root;
    if (previous == nullptr) {
        root->front = new_node;
    } else {
        previous->next = new_node;
    }
    previous = new_node;
}

template <class T>
void gearoenix::core::list::Node::add_as_chid(const T& next_value)
{
    Node<T>* new_node = new Node(next_value);
    new_node->next = next;
    new_node->previous = this;
    new_node->root = root;
    if (next == nullptr) {
        root->end = new_node;
    } else {
        next->previous = new_node;
    }
    next = new_node;
}
#endif
