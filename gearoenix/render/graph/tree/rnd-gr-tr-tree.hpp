#ifndef GEAROENIX_RENDER_GRAPH_TREE_TREE_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_TREE_HPP
#include <memory>
namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace graph::tree {
    class Tree {
    protected:
        engine::Engine* e = nullptr;
        explicit Tree(engine::Engine* const e) noexcept
            : e(e)
        {
        }

    public:
        virtual ~Tree() noexcept = default;
        /// It is called from graphic thread
        virtual void update() noexcept = 0;
        /// Multithreaded rendering happens in here
        virtual void record(unsigned int kernel_index) noexcept = 0;
        /// Submit is called from graphic thread
        virtual void submit() noexcept = 0;
    };
}
}
#endif