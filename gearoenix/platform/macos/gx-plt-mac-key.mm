#include "gx-plt-mac-key.hpp"
#ifdef GX_PLATFORM_MACOS
#import <array>

gearoenix::platform::key::Id gearoenix::platform::convert_key(const unsigned short key_code) noexcept
{
    static const std::array<key::Id, 0x80> ks = []() constexpr
    {
        std::array<key::Id, 0x80> ks = { key::Id::Unknown };
#define GX_HELPER(ch, num) ks[num] = key::Id::ch
        GX_HELPER(A, 0x00);
        GX_HELPER(S, 0x01);
        GX_HELPER(D, 0x02);
        GX_HELPER(F, 0x03);
        GX_HELPER(H, 0x04);
        GX_HELPER(G, 0x05);
        GX_HELPER(Z, 0x06);
        GX_HELPER(X, 0x07);
        GX_HELPER(C, 0x08);
        GX_HELPER(V, 0x09);
        GX_HELPER(B, 0x0B);
        GX_HELPER(Q, 0x0C);
        GX_HELPER(W, 0x0D);
        GX_HELPER(E, 0x0E);
        GX_HELPER(R, 0x0F);
        GX_HELPER(Y, 0x10);
        GX_HELPER(T, 0x11);
        GX_HELPER(Num1, 0x12);
        GX_HELPER(Num2, 0x13);
        GX_HELPER(Num3, 0x14);
        GX_HELPER(Num4, 0x15);
        GX_HELPER(Num6, 0x16);
        GX_HELPER(Num5, 0x17);
        GX_HELPER(Equal, 0x18);
        GX_HELPER(Num9, 0x19);
        GX_HELPER(Num7, 0x1A);
        GX_HELPER(Minus, 0x1B);
        GX_HELPER(Num8, 0x1C);
        GX_HELPER(Num0, 0x1D);
        GX_HELPER(RightBracket, 0x1E);
        GX_HELPER(O, 0x1F);
        GX_HELPER(U, 0x20);
        GX_HELPER(LeftBracket, 0x21);
        GX_HELPER(I, 0x22);
        GX_HELPER(P, 0x23);
        GX_HELPER(Enter, 0x24);
        GX_HELPER(L, 0x25);
        GX_HELPER(J, 0x26);
        GX_HELPER(Quote, 0x27);
        GX_HELPER(K, 0x28);
        GX_HELPER(Semicolon, 0x29);
        GX_HELPER(Backslash, 0x2A);
        GX_HELPER(Comma, 0x2B);
        GX_HELPER(Slash, 0x2C);
        GX_HELPER(N, 0x2D);
        GX_HELPER(M, 0x2E);
        GX_HELPER(Dot, 0x2F);
        GX_HELPER(Tab, 0x30);
        GX_HELPER(Space, 0x31);
        GX_HELPER(Grave, 0x32);
        GX_HELPER(Delete, 0x33);
        GX_HELPER(Escape, 0x35);
        GX_HELPER(LeftSuper, 0x37);
        GX_HELPER(LeftShift, 0x38);
        GX_HELPER(CapsLock, 0x39);
        GX_HELPER(LeftAlt, 0x3A);
        GX_HELPER(LeftControl, 0x3B);
        GX_HELPER(RightShift, 0x3C);
        GX_HELPER(RightAlt, 0x3D);
        GX_HELPER(RightControl, 0x3E);
        GX_HELPER(Function, 0x3F);
        GX_HELPER(F17, 0x40);
        GX_HELPER(NumpadDecimal, 0x41);
        GX_HELPER(NumpadStar, 0x43);
        GX_HELPER(NumpadPlus, 0x45);
        GX_HELPER(NumpadClear, 0x47);
        GX_HELPER(VolumeUp, 0x48);
        GX_HELPER(VolumeDown, 0x49);
        GX_HELPER(Mute, 0x4A);
        GX_HELPER(F18, 0x4F);
        GX_HELPER(NumpadSlash, 0x4B);
        GX_HELPER(NumpadEnter, 0x4C);
        GX_HELPER(NumpadMinus, 0x4E);
        GX_HELPER(F19, 0x50);
        GX_HELPER(NumpadEquals, 0x51);
        GX_HELPER(Numpad0, 0x52);
        GX_HELPER(Numpad1, 0x53);
        GX_HELPER(Numpad2, 0x54);
        GX_HELPER(Numpad3, 0x55);
        GX_HELPER(Numpad4, 0x56);
        GX_HELPER(Numpad5, 0x57);
        GX_HELPER(Numpad6, 0x58);
        GX_HELPER(Numpad7, 0x59);
        GX_HELPER(F20, 0x5A);
        GX_HELPER(Numpad8, 0x5B);
        GX_HELPER(Numpad9, 0x5C);
        GX_HELPER(F5, 0x60);
        GX_HELPER(F6, 0x61);
        GX_HELPER(F7, 0x62);
        GX_HELPER(F3, 0x63);
        GX_HELPER(F8, 0x64);
        GX_HELPER(F9, 0x65);
        GX_HELPER(F11, 0x67);
        GX_HELPER(F13, 0x69);
        GX_HELPER(F16, 0x6A);
        GX_HELPER(F14, 0x6B);
        GX_HELPER(F10, 0x6D);
        GX_HELPER(F12, 0x6F);
        GX_HELPER(F15, 0x71);
        GX_HELPER(Help, 0x72);
        GX_HELPER(Home, 0x73);
        GX_HELPER(PageUp, 0x74);
        GX_HELPER(Delete, 0x75);
        GX_HELPER(F4, 0x76);
        GX_HELPER(End, 0x77);
        GX_HELPER(F2, 0x78);
        GX_HELPER(PageDown, 0x79);
        GX_HELPER(F1, 0x7A);
        GX_HELPER(Left, 0x7B);
        GX_HELPER(Right, 0x7C);
        GX_HELPER(Down, 0x7D);
        GX_HELPER(Up, 0x7E);
        return ks;
    }
    ();
    if (ks.size() <= key_code)
        return key::Id::Unknown;
    return ks[key_code];
}

#endif
