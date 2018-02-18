#ifndef GEAROENIX_SYSTEM_STREAM_STREAM_HPP
#define GEAROENIX_SYSTEM_STREAM_STREAM_HPP
#include "../../core/cr-types.hpp"
#include <string>
#include <type_traits>
#include <vector>
namespace gearoenix {
namespace system {
    class Application;
    namespace stream {
        class Stream {
        private:
        protected:
            Stream();
            virtual void built_in_type_read(void* data, core::Count length);

        public:
            virtual ~Stream();
            virtual core::Count read(void* data, core::Count length) = 0;
            virtual core::Count write(const void* data, core::Count length) = 0;
            virtual void seek(core::Count offset) = 0;
            virtual core::Count tell() = 0;

            std::string read_string();
            bool read_bool();

            template <typename T>
            void read(std::vector<T>& data)
            {
                core::Count c;
                read(c);
                data.resize((size_t)c);
                for (core::Count i = 0; i < c; ++i) {
                    read(data[i]);
                }
            }

            template <typename T>
            typename std::enable_if<sizeof(T) == 1 && (std::is_integral<T>::value || std::is_enum<T>::value), void>::type
            read(T& data)
            {
                read(&data, sizeof(T));
            }

            template <typename T>
            typename std::enable_if<(sizeof(T) > 1) && (sizeof(T) < 9) && (std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_enum<T>::value), void>::type
            read(T& data)
            {
                built_in_type_read(&data, sizeof(T));
            }

            template <typename T>
            T read()
            {
                T data;
                read(data);
                return data;
            }

            template <typename T>
            void write(const T& d)
            {
                write(&d, sizeof(T));
            }
        };
    }
}
}
#endif
