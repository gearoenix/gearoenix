#ifndef GEAROENIX_RENDER_TEXTURE_PIXEL_ITERATOR_HPP
#define GEAROENIX_RENDER_TEXTURE_PIXEL_ITERATOR_HPP
#include <cstdint>
#include <vector>

namespace gearoenix::render::texture {
template <typename Element>
struct Pixel {
    struct ConstIterator {
        using difference_type = std::size_t;
        using value_type = const Element;
        using pointer = const Element*;
        using reference = const Element&;
        using iterator_category = std::random_access_iterator_tag;

        constexpr ConstIterator(const Element* data, std::size_t components_count, std::size_t pixel_counts) noexcept
            : data(data)
            , pixels_count(pixel_counts)
            , elements_count(components_count * pixel_counts)
            , pixel_size(components_count * sizeof(Element))
            , current_ptr(reinterpret_cast<std::size_t>(data))
            , end_ptr(current_ptr + pixel_size * pixel_counts)
        {
        }

        constexpr ConstIterator(const Element* data, std::size_t components_count, std::size_t pixel_counts, std::size_t elements_count) noexcept
            : data(data)
            , pixels_count(pixel_counts)
            , elements_count(elements_count)
            , pixel_size(components_count * sizeof(Element))
            , current_ptr(reinterpret_cast<std::size_t>(data))
            , end_ptr(current_ptr + pixel_size * pixel_counts)
        {
        }

        constexpr ConstIterator(const ConstIterator& o) noexcept = default;

        constexpr ConstIterator& operator++() noexcept
        {
            current_ptr += pixel_size;
            ++pixel_index;
            if (current_ptr > end_ptr) {
                current_ptr = end_ptr;
                pixel_index = pixels_count;
            }
        }

        template <typename I>
        constexpr void operator+=(const I i) noexcept
        {
            current_ptr += pixel_size * static_cast<std::size_t>(i);
            pixel_index += static_cast<std::size_t>(i);
            if (current_ptr > end_ptr) {
                current_ptr = end_ptr;
                pixel_index = pixels_count;
            }
        }

        template <typename I>
        constexpr ConstIterator operator+(const I i) const noexcept
        {
            ConstIterator o(*this);
            o += i;
            return o;
        }

        template <typename I>
        constexpr Element operator[](const I i) const noexcept
        {
            return reinterpret_cast<const Element*>(current_ptr)[i];
        }

        [[nodiscard]] constexpr reference operator*() noexcept
        {
            return *reinterpret_cast<const Element*>(current_ptr);
        }

        [[nodiscard]] constexpr pointer operator->() noexcept
        {
            return reinterpret_cast<const Element*>(current_ptr);
        }

        [[nodiscard]] constexpr bool operator==(const ConstIterator& rhs) const noexcept
        {
            return current_ptr == rhs.current_ptr;
        }

        [[nodiscard]] constexpr bool operator!=(const ConstIterator& rhs) const noexcept
        {
            return current_ptr != rhs.current_ptr;
        }

        [[nodiscard]] constexpr bool operator<(const ConstIterator& rhs) const noexcept
        {
            return current_ptr < rhs.current_ptr;
        }

        [[nodiscard]] constexpr bool operator<=(const ConstIterator& rhs) const noexcept
        {
            return current_ptr <= rhs.current_ptr;
        }

        [[nodiscard]] constexpr bool operator>(const ConstIterator& rhs) const noexcept
        {
            return current_ptr > rhs.current_ptr;
        }

        [[nodiscard]] constexpr bool operator>=(const ConstIterator& rhs) const noexcept
        {
            return current_ptr > rhs.current_ptr;
        }

        [[nodiscard]] constexpr std::size_t get_pixel_index() const noexcept
        {
            return pixel_index;
        }

    private:
        const Element* const data;
        const std::size_t pixels_count;
        const std::size_t elements_count;
        const std::size_t pixel_size;
        std::size_t pixel_index = 0;
        std::size_t current_ptr;
        const std::size_t end_ptr;
    };
};
}
#endif
