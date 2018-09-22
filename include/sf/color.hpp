//StreamFormat color.hpp
#ifndef SF_COLOR_HPP
#define SF_COLOR_HPP

#include <sf/utility.hpp>

#if defined(SF_WIN_NATIVE_COLOR) || (defined(SF_CXX17) || _MSC_VER >= 1910)

#ifdef SF_WIN_NATIVE_COLOR
#include <Windows.h>
#else
#include <sf/ansi.hpp>
#include <variant>
#endif // SF_WIN_NATIVE_COLOR

namespace sf
{
#ifndef SF_WIN_NATIVE_COLOR
    enum sgr_chars : unsigned char
    {
        normal,
        bold,
        faint,
        italic,
        underline,
        blink,
        rapid,
        reverse_video,
        conceal,
        crossed,
        primary,
        fraktur = 20,
        double_underline,
        bold_off = 21,
        intensity,
        italic_off,
        fraktur_off = 23,
        underline_off,
        blink_off,
        inverse_off = 27,
        conceal_off,
        crossed_off,
        foreground = 38,
        background = 48,
        framed = 51,
        encircled,
        overlined,
        framed_off,
        encircled_off = 54,
        overlined_off
    };

    enum preset_color : unsigned char
    {
        black = 30,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
        user_default = 39, //To make clang happy, use `user_default` instead of `default` keyword.
        bright_black = 90,
        bright_red,
        bright_green,
        bright_yellow,
        bright_blue,
        bright_magenta,
        bright_cyan,
        bright_white
    };

    struct rgb_color
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        rgb_color() = default;
        SF_CONSTEXPR rgb_color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
    };

    namespace internal
    {
        class color
        {
        public:
            typedef std::variant<preset_color, unsigned char, rgb_color> color_type;

        private:
            color_type value;
            bool isback;

        public:
            color() = default;
            color(color_type value, bool isback) : value(value), isback(isback) {}
            template <typename Char, typename Traits>
            friend SF_CONSTEXPR std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const color& c)
            {
                switch (c.value.index())
                {
                case 0: //preset_color
                    stream << (static_cast<int>(std::get<preset_color>(c.value)) + (c.isback ? 10 : 0));
                    break;
                case 1: //unsigned char
                    join_args(stream, static_cast<int>(c.isback ? background : foreground), 5, static_cast<int>(std::get<unsigned char>(c.value)));
                    break;
                case 2: //rgb_color
                    rgb_color rgb = std::get<rgb_color>(c.value);
                    join_args(stream, static_cast<int>(c.isback ? background : foreground), 2, static_cast<int>(rgb.r), static_cast<int>(rgb.g), static_cast<int>(rgb.b));
                    break;
                }
                return stream;
            }
        };

        //Pack an arg with its foreground and background color.
        template <typename T>
        class color_arg
        {
        public:
            typedef color::color_type color_type;

        private:
            T arg;
            color fore, back;
            sgr_chars sgr;

        public:
            SF_CONSTEXPR color_arg() : fore(user_default, false), back(user_default, true), sgr(normal) {}
            SF_CONSTEXPR color_arg(T&& arg, color_type fore, color_type back, sgr_chars sgr) : arg(arg), fore(fore, false), back(back, true), sgr(sgr) {}
            template <typename Char, typename Traits>
            friend SF_CONSTEXPR std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const color_arg<T>& arg)
            {
                return stream << make_ansi_control(static_cast<int>(arg.sgr), arg.fore, arg.back) << arg.arg << make_ansi_control();
            }
        };
    } // namespace internal

    using color_type = typename internal::color::color_type;
#else
    enum sgr_chars : WORD
    {
        normal = 0x0,
        bold = 0x8 //bright
    };
    enum preset_color : WORD
    {
        black = 0x0,
        blue = 0x1,
        green = 0x2,
        cyan = blue | green,
        red = 0x4,
        magenta = blue | red,
        yellow = green | red,
        white = blue | green | red,
        bright_base = 0x8,
        bright_black = black | bright_base,
        bright_blue = blue | bright_base,
        bright_green = green | bright_base,
        bright_cyan = cyan | bright_base,
        bright_red = red | bright_base,
        bright_magenta = magenta | bright_base,
        bright_yellow = yellow | bright_base,
        bright_white = white | bright_base,
        color_mask = 0xF,
        background_base = 0x10,
        user_default = 0x100
    };

    namespace internal
    {
        //Pack an arg with its foreground and background color.
        template <typename T>
        class color_arg
        {
        private:
            T arg;
            preset_color fore, back;
            sgr_chars sgr;

        public:
            SF_CONSTEXPR color_arg() : fore(user_default), back(user_default), sgr(normal) {}
            SF_CONSTEXPR color_arg(T&& arg, preset_color fore, preset_color back, sgr_chars sgr) : arg(arg), fore(fore), back(back), sgr(sgr) {}
            template <typename Char, typename Traits>
            friend SF_CONSTEXPR std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const color_arg<T>& arg)
            {
                HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
                CONSOLE_SCREEN_BUFFER_INFO info;
                WORD def = static_cast<WORD>(white) | (static_cast<WORD>(black) * background_base);
                if (GetConsoleScreenBufferInfo(hc, &info))
                {
                    def = info.wAttributes;
                }
                WORD fw = (arg.fore == user_default ? (def & color_mask) : arg.fore) | arg.sgr;
                WORD bw = arg.back == user_default ? (def & (color_mask * background_base)) : arg.back * background_base;
                SetConsoleTextAttribute(hc, fw | bw);
                stream << arg.arg;
                SetConsoleTextAttribute(hc, def);
                return stream;
            }
        };
    } // namespace internal

    using color_type = preset_color;
#endif // !SF_WIN_NATIVE_COLOR

    template <typename T>
    SF_CONSTEXPR internal::color_arg<T> make_color_arg(T&& arg, color_type fore, color_type back = user_default, sgr_chars sgr = normal)
    {
        return internal::color_arg<T>(std::forward<T>(arg), fore, back, sgr);
    }
    template <typename T>
    SF_CONSTEXPR internal::color_arg<T> make_color_arg(T&& arg, sgr_chars sgr)
    {
        return internal::color_arg<T>(std::forward<T>(arg), user_default, user_default, sgr);
    }
} // namespace sf

#endif // SF_WIN_NATIVE_COLOR || SF_CXX17

#endif // !SF_COLOR_HPP