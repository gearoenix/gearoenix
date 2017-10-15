#ifndef GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#define GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace buffer {
        class SubBuffer;
        class Uniform {
        private:
            unsigned int us;
            unsigned int uc;
            SubBuffer** vbuf;
            SubBuffer** cbuf;

        public:
            Uniform(unsigned int us, Engine* e);
            ~Uniform();
            const SubBuffer* const* get_vbuf() const;
            const SubBuffer* const* get_cbuf() const;
            unsigned int get_count() const;
            unsigned int get_size() const;
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
