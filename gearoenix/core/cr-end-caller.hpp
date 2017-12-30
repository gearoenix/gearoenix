#ifndef GEAROENIX_CORE_END_CALLER_HPP
#define GEAROENIX_CORE_END_CALLER_HPP
#include <functional>
#include <memory>
#include "cr-build-configuration.hpp"
#include "../system/sys-log.hpp"

namespace gearoenix {
namespace core {
    template <typename T>
    class EndCaller {
    private:
        class Caller {
        public:
            std::function<void(std::shared_ptr<T>)> f;
            std::shared_ptr<T> data = nullptr;
            Caller(std::function<void(std::shared_ptr<T>)> f): f(f){}
            ~Caller() {
#ifdef DEBUG_MODE
                if(nullptr == data) UNEXPECTED;
#endif
                f(data);
            }
        };
        std::shared_ptr<Caller> caller;
    public:
        EndCaller(std::function<void(std::shared_ptr<T>)> f): caller(new Caller(f)) {}
        void set_data(std::shared_ptr<T> data) {
            caller->data = data;
        }
    };
    typedef struct {} EndCallerIgnore;
#ifdef DEBUG_MODE
    template <>
    class EndCaller<EndCallerIgnore> {
    private:
        class Caller {
        public:
            std::function<void(std::shared_ptr<EndCallerIgnore>)> f;
            std::shared_ptr<EndCallerIgnore> data = nullptr;
            Caller(std::function<void(std::shared_ptr<EndCallerIgnore>)> f): f(f){}
            ~Caller() {
                f(data);
            }
        };
        std::shared_ptr<Caller> caller;
    public:
        EndCaller(std::function<void(std::shared_ptr<EndCallerIgnore>)> f): caller(new Caller(f)) {}
        void set_data(std::shared_ptr<EndCallerIgnore> data) {
            caller->data = data;
        }
    };
#endif
}
}
#endif
