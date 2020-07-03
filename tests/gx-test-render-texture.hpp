#include <algorithm>
#include <chrono>
#include <gearoenix/render/texture/rnd-txt-texture.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(render_texture_test)
{

    static std::random_device rd;
    static std::default_random_engine gen(rd());
    static std::uniform_real_distribution<float> dis(-0.5f, 1.5f);

    constexpr std::size_t aspect = 32;
    constexpr std::size_t components_count = 4;

    std::vector<std::vector<std::vector<float>>> float_data(6);
    std::vector<std::vector<std::vector<std::uint8_t>>> data(float_data.size());
    std::vector<std::vector<std::vector<std::uint8_t>>> compare_data(float_data.size());
    for (std::size_t face_index = 0; face_index < float_data.size(); ++face_index) {
        auto& float_face_data = float_data[face_index];
        auto& face_data = data[face_index];
        auto& compare_face_data = compare_data[face_index];
        float_face_data.resize(gearoenix::math::Numeric::floor_log2(aspect) + 1);
        face_data.resize(float_face_data.size());
        compare_face_data.resize(float_face_data.size());
        for (std::size_t level_index = 0; level_index < face_data.size(); ++level_index) {
            auto& float_level_data = float_face_data[level_index];
            auto& level_data = face_data[level_index];
            auto& compare_level_data = compare_face_data[level_index];
            float_level_data.resize((aspect >> level_index) * (aspect >> level_index) * components_count);
            level_data.resize(float_level_data.size() * sizeof(float_level_data[0]));
            compare_level_data.resize(float_level_data.size());
            for (std::size_t element_index = 0; element_index < float_level_data.size(); ++element_index) {
                const float c = dis(gen);
                float_level_data[element_index] = c;
                const float cc = std::round(c * 255.0f) + 0.1f;
                compare_level_data[element_index] = cc >= 255.0f ? 255 : cc <= 0.0f ? 0 : static_cast<std::uint8_t>(cc);
            }
            std::memcpy(level_data.data(), float_level_data.data(), level_data.size());
        }
    }

    const auto result = gearoenix::render::texture::Texture::convert_float_pixels(data, components_count, components_count);

    BOOST_TEST_MESSAGE("Going to test the result from gearoenix::render::texture::Texture::convert_float_pixels");
    BOOST_TEST(result.size() == compare_data.size());
    for (std::size_t face_index = 0; face_index < compare_data.size(); ++face_index) {
        auto& face_result = result[face_index];
        auto& compare_face_data = compare_data[face_index];
        BOOST_TEST(face_result.size() == compare_face_data.size());
        for (std::size_t level_index = 0; level_index < compare_face_data.size(); ++level_index) {
            auto& level_result = face_result[level_index];
            auto& compare_level_data = compare_face_data[level_index];
            BOOST_TEST(level_result.size() == compare_level_data.size());
            for (std::size_t element_index = 0; element_index < compare_level_data.size(); ++element_index) {
                BOOST_TEST(level_result[element_index] == compare_level_data[element_index],
                    "Index: " << element_index << ", comp: " << (element_index % components_count) << ", pixel: " << (element_index / components_count) << ", correct: " << int(compare_level_data[element_index]) << ", wrong: " << int(level_result[element_index]));
            }
        }
    }
}