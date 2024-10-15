#ifndef GEAROENIX_EDITOR_UI_UTILITY_HPP
#define GEAROENIX_EDITOR_UI_UTILITY_HPP

namespace gearoenix::editor::ui {
struct Utility final {
    struct WrongInputTextStyle final {
    private:
        const bool is_invalid;

    public:
        explicit WrongInputTextStyle(bool is_valid);
        ~WrongInputTextStyle();
    };

    [[nodiscard]] static WrongInputTextStyle set_wrong_input_text(bool is_valid);
};
}

#endif