#pragma once

namespace gearoenix::render::texture {
template <typename Element>
struct Pixel {
    struct ConstIterator {
        using difference_type = std::uint64_t;
        using value_type = const Element;
        using pointer = const Element*;
        using reference = const Element&;
        using iterator_category = std::random_access_iterator_tag;

        constexpr ConstIterator() = default;

        constexpr ConstIterator(const Element* data, const std::uint64_t components_count, const std::uint64_t pixel_counts)
            : data(data)
            , pixels_count(pixel_counts)
            , elements_count(components_count * pixel_counts)
            , pixel_size(components_count * sizeof(Element))
            , current_ptr(reinterpret_cast<std::uint64_t>(data))
            , end_ptr(current_ptr + pixel_size * pixel_counts)
        {
        }

        constexpr ConstIterator(const Element* data, const std::uint64_t components_count, const std::uint64_t pixel_counts, const std::uint64_t elements_count)
            : data(data)
            , pixels_count(pixel_counts)
            , elements_count(elements_count)
            , pixel_size(components_count * sizeof(Element))
            , current_ptr(reinterpret_cast<std::uint64_t>(data))
            , end_ptr(current_ptr + pixel_size * pixel_counts)
        {
        }

        constexpr ConstIterator(const ConstIterator& o) = default;

        constexpr ConstIterator& operator++()
        {
            current_ptr += pixel_size;
            ++pixel_index;
            if (current_ptr > end_ptr) {
                current_ptr = end_ptr;
                pixel_index = pixels_count;
            }
            return *this;
        }

        template <typename I>
        constexpr void operator+=(const I i)
        {
            current_ptr += pixel_size * static_cast<std::uint64_t>(i);
            pixel_index += static_cast<std::uint64_t>(i);
            if (current_ptr > end_ptr) {
                current_ptr = end_ptr;
                pixel_index = pixels_count;
            }
        }

        template <typename I>
        constexpr ConstIterator operator+(const I i) const
        {
            ConstIterator o(*this);
            o += i;
            return o;
        }

        [[nodiscard]] constexpr difference_type operator-(const ConstIterator& o) const { return pixel_index - o.pixel_index; }

        template <typename I>
        constexpr const Element& operator[](const I i) const { return reinterpret_cast<const Element*>(current_ptr)[i]; }

        [[nodiscard]] constexpr reference operator*() { return *reinterpret_cast<const Element*>(current_ptr); }

        [[nodiscard]] constexpr pointer operator->() { return reinterpret_cast<const Element*>(current_ptr); }

        [[nodiscard]] constexpr bool operator==(const ConstIterator& rhs) const { return current_ptr == rhs.current_ptr; }

        [[nodiscard]] constexpr bool operator!=(const ConstIterator& rhs) const { return current_ptr != rhs.current_ptr; }

        [[nodiscard]] constexpr bool operator<(const ConstIterator& rhs) const { return current_ptr < rhs.current_ptr; }

        [[nodiscard]] constexpr bool operator<=(const ConstIterator& rhs) const { return current_ptr <= rhs.current_ptr; }

        [[nodiscard]] constexpr bool operator>(const ConstIterator& rhs) const { return current_ptr > rhs.current_ptr; }

        [[nodiscard]] constexpr bool operator>=(const ConstIterator& rhs) const { return current_ptr > rhs.current_ptr; }

        [[nodiscard]] constexpr std::uint64_t get_pixel_index() const { return pixel_index; }

        [[nodiscard]] constexpr const Element* get_data() const { return data; }

    private:
        const Element* data = nullptr;
        std::uint64_t pixels_count = 0;
        std::uint64_t elements_count = 0;
        std::uint64_t pixel_size = 0;
        std::uint64_t pixel_index = 0;
        std::uint64_t current_ptr = 0;
        std::uint64_t end_ptr = 0;
    };
};
}