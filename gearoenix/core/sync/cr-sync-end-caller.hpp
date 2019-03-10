#ifndef GEAROENIX_CORE_SYNC_END_CALLER_HPP
#define GEAROENIX_CORE_SYNC_END_CALLER_HPP
#include "../../system/sys-log.hpp"
#include "../cr-build-configuration.hpp"
#include <functional>
#include <memory>

namespace gearoenix {
namespace core {
    namespace sync {
        template <typename T>
        class EndCaller {
        private:
            class Caller {
            public:
                std::function<void(std::shared_ptr<T>)> f;
                std::shared_ptr<T> data = nullptr;
                Caller(std::function<void(std::shared_ptr<T>)> f)
                    : f(f)
                {
                }
                ~Caller()
                {
#ifdef GX_DEBUG_MODE
                    if (nullptr == data)
                        GXUNEXPECTED;
#endif
                    f(data);
                }
            };
            std::shared_ptr<Caller> caller;

        public:
            EndCaller(std::function<void(std::shared_ptr<T>)> f)
                : caller(new Caller(f))
            {
            }
            template <typename V>
            EndCaller(V v)
                : caller(new Caller([v](std::shared_ptr<T>) -> void {}))
            {
            }
            EndCaller(const EndCaller& o)
                : caller(o.caller)
            {
            }
            void set_data(const std::shared_ptr<T>& data)
            {
                caller->data = data;
            }
        };
        typedef struct {
        } EndCallerIgnore;
#ifdef GX_DEBUG_MODE
        template <>
        class EndCaller<EndCallerIgnore> {
        private:
            class Caller {
            public:
                std::function<void(std::shared_ptr<EndCallerIgnore>)> f;
                std::shared_ptr<EndCallerIgnore> data = nullptr;
                Caller(std::function<void(std::shared_ptr<EndCallerIgnore>)> f)
                    : f(f)
                {
                }
                ~Caller()
                {
                    f(data);
                }
            };
            std::shared_ptr<Caller> caller;

        public:
            EndCaller(std::function<void(std::shared_ptr<EndCallerIgnore>)> f)
                : caller(new Caller(f))
            {
            }
			template <typename V>
			EndCaller(V v)
                : caller(new Caller([v](std::shared_ptr<EndCallerIgnore>) -> void {}))
			{
			}
            void set_data(std::shared_ptr<EndCallerIgnore> data)
            {
                caller->data = data;
            }
        };
#endif
    }
}
}
#endif
