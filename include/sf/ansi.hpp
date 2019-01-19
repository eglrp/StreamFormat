/**StreamFormat ansi.hpp
 * 
 * MIT License
 * 
 * Copyright (c) 2018-2019 Berrysoft
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */
#ifndef SF_ANSI_HPP
#define SF_ANSI_HPP

#include <sf/utility.hpp>

#include <ostream>
#include <tuple>

namespace sf
{
    namespace internal
    {
        SF_CHAR_TEMPLATE(esc, '\033')
        SF_CHAR_TEMPLATE(sqr_bra, '[')
        SF_CHAR_TEMPLATE(smcolon, ';')

        template <typename Char, typename Traits, typename Arg0, typename... Args>
        constexpr std::basic_ostream<Char, Traits>& join_args(std::basic_ostream<Char, Traits>& stream, Arg0&& arg0, Args&&... args)
        {
            if constexpr (sizeof...(Args) > 0)
                return join_args(stream << std::forward<Arg0>(arg0) << smcolon<Char>, std::forward<Args>(args)...);
            else
                return stream << std::forward<Arg0>(arg0);
        }

        template <typename Char, typename Traits, typename... Args, std::size_t... Indeces>
        constexpr std::basic_ostream<Char, Traits>& join_args_helper(std::basic_ostream<Char, Traits>& stream, const std::tuple<Args...>& args, std::index_sequence<Indeces...>)
        {
            return join_args(stream, std::get<Indeces>(args)...);
        }

        template <typename Char, typename Traits, typename... Args>
        constexpr std::basic_ostream<Char, Traits>& write_ansi(std::basic_ostream<Char, Traits>& stream, const std::tuple<Args...>& args)
        {
            if constexpr (sizeof...(Args) > 0)
                return join_args_helper(stream << esc<Char> << sqr_bra<Char>, args, std::make_index_sequence<sizeof...(Args)>{});
            else
                return stream << esc<Char> << sqr_bra<Char>;
        }

        //Write Console Virtual Terminal Sequences (ANSI Control Characters) to a stream.
        template <typename C, typename... Args>
        class ansi_control
        {
        private:
            C endc;
            std::tuple<Args...> args;

        public:
            ansi_control(C endc, Args&&... args) : endc(endc), args(std::forward<Args>(args)...) {}
            template <typename Char, typename Traits = std::char_traits<Char>>
            friend constexpr std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const ansi_control& ctrl)
            {
                return write_ansi(stream, ctrl.args) << ctrl.endc;
            }
        };

        template <typename Char, typename... Args>
        constexpr ansi_control<Char, Args...> make_ansi_control(Char endc, Args&&... args)
        {
            return ansi_control<Char, Args...>(endc, std::forward<Args>(args)...);
        }

        SF_CHAR_TEMPLATE(cuu_end, 'A')
        SF_CHAR_TEMPLATE(cud_end, 'B')
        SF_CHAR_TEMPLATE(cuf_end, 'C')
        SF_CHAR_TEMPLATE(cub_end, 'D')
        SF_CHAR_TEMPLATE(cnl_end, 'E')
        SF_CHAR_TEMPLATE(cpl_end, 'F')
        SF_CHAR_TEMPLATE(cha_end, 'G')
        SF_CHAR_TEMPLATE(cup_end, 'H')
        SF_CHAR_TEMPLATE(ed_end, 'J')
        SF_CHAR_TEMPLATE(el_end, 'K')
        SF_CHAR_TEMPLATE(su_end, 'S')
        SF_CHAR_TEMPLATE(sd_end, 'T')
        SF_CHAR_TEMPLATE(sgr_end, 'm')
        SF_CHAR_TEMPLATE(dsr_end, 'n')
        SF_CHAR_TEMPLATE(scp_end, 's')
        SF_CHAR_TEMPLATE(rcp_end, 'u')
    } // namespace internal

#define SF_MAKE_MOVE(name, charname)                                                \
    template <typename Char>                                                        \
    constexpr internal::ansi_control<Char, std::size_t> name(std::size_t n = 1)     \
    {                                                                               \
        return internal::make_ansi_control(internal::charname<Char>, std::move(n)); \
    }

    SF_MAKE_MOVE(make_cursor_upward, cuu_end)
    SF_MAKE_MOVE(make_cursor_downward, cud_end)
    SF_MAKE_MOVE(make_cursor_forward, cuf_end)
    SF_MAKE_MOVE(make_cursor_backward, cub_end)

    SF_MAKE_MOVE(make_cursor_next_line, cnl_end)
    SF_MAKE_MOVE(make_cursor_pre_line, cpl_end)
    SF_MAKE_MOVE(make_cursor_abs_line, cha_end)

    template <typename Char>
    constexpr internal::ansi_control<Char, std::size_t, std::size_t> make_cursor_set_pos(std::size_t line = 1, std::size_t index = 1)
    {
        return internal::make_ansi_control(internal::cup_end<Char>, std::move(line), std::move(index));
    }

    enum erase_opt
    {
        erase_to_end,
        erase_to_start,
        erase_all,
        erase_all_buffer // Only apply to ED.
    };

    template <typename Char>
    constexpr internal::ansi_control<Char, int> make_erase_screen(erase_opt opt)
    {
        return internal::make_ansi_control(internal::ed_end<Char>, static_cast<int>(opt));
    }

    template <typename Char>
    constexpr internal::ansi_control<Char, int> make_erase_line(erase_opt opt)
    {
        return internal::make_ansi_control(internal::el_end<Char>, static_cast<int>(opt));
    }

    SF_MAKE_MOVE(make_scroll_up, su_end)
    SF_MAKE_MOVE(make_scroll_down, sd_end)

    template <typename Char, typename... Args>
    constexpr internal::ansi_control<Char, Args...> make_sgr_control(Args&&... args)
    {
        return internal::make_ansi_control(internal::sgr_end<Char>, std::forward<Args>(args)...);
    }

    template <typename Char>
    constexpr internal::ansi_control<Char, int> make_cursor_pos_report()
    {
        return internal::make_ansi_control(internal::dsr_end<Char>, 6);
    }

    template <typename Char>
    constexpr internal::ansi_control<Char> make_cursor_save()
    {
        return internal::make_ansi_control(internal::scp_end<Char>);
    }
    template <typename Char>
    constexpr internal::ansi_control<Char> make_cursor_restore()
    {
        return internal::make_ansi_control(internal::rcp_end<Char>);
    }
} // namespace sf

#endif // !SF_ANSI_HPP
