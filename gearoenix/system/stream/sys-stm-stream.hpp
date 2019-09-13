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
            bool is_endian_compatible = true;

            Stream() noexcept {}

            void built_in_type_read(void* data, core::Count length) noexcept
            {
                read(data, length);
                if (is_endian_compatible)
                    return;
                std::uint8_t* c_data = static_cast<std::uint8_t*>(data);
                for (core::Count i = 0, j = length - 1; i < j; ++i, --j) {
                    std::uint8_t tmp = c_data[i];
                    c_data[i] = c_data[j];
                    c_data[j] = tmp;
                }
            }

        public:
            virtual ~Stream() noexcept {}

            virtual core::Count read(void* data, core::Count length) noexcept = 0;
            virtual core::Count write(const void* data, core::Count length) noexcept = 0;
            virtual void seek(core::Count offset) noexcept = 0;
            virtual core::Count tell() noexcept = 0;

            bool get_endian_compatibility() const noexcept
            {
                return is_endian_compatible;
            }

            std::string read_string() noexcept
            {
                core::Count c;
                read(c);
                std::string s;
                s.resize(c);
                read(&(s[0]), c);
                return s;
            }

            bool read_bool() noexcept
            {
                std::uint8_t data;
                read(&data, 1);
                return data != 0;
            }

            template <typename T>
            typename std::enable_if<sizeof(T) != 1, void>::type
            read(std::vector<T>& data) noexcept
            {
                core::Count c;
                read(c);
                data.resize((size_t)c);
                for (core::Count i = 0; i < c; ++i) {
                    read(data[i]);
                }
            }

            template <typename T>
            typename std::enable_if<sizeof(T) == 1, void>::type
            read(std::vector<T>& data) noexcept
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
            read(T& data) noexcept
            {
                read(&data, sizeof(T));
            }

            template <typename T>
            typename std::enable_if<(sizeof(T) > 1) && (sizeof(T) < 9) && (std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_enum<T>::value), void>::type
            read(T& data) noexcept
            {
                built_in_type_read(&data, sizeof(T));
            }

            template <typename T>
            T read() noexcept
            {
                T data;
                read(data);
                return data;
            }

            template <typename T>
            void write(const T& d) noexcept
            {
                write(&d, sizeof(T));
            }
        };
    }
}
}
#endif
