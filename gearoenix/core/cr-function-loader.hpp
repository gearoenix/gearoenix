#ifndef GEAROENIX_CORE_FUNCTIONLOADER_HPP
#define GEAROENIX_CORE_FUNCTIONLOADER_HPP
#include <functional>
#include <mutex>
#include <vector>
namespace gearoenix {
namespace core {

    class FunctionLoader {
    private:
        std::mutex locker;
        std::vector<std::function<void()>> load_functions;

    public:
        FunctionLoader();
        ~FunctionLoader();
        void load(std::function<void()> fun);
        void unload_one();
        void unload_all();
        unsigned int get_loadeds_count() const;
    };

} // namespace core
} // namespace gearoenix

#endif // CRFUNCTIONLOADER_HPP
