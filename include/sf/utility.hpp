/**StreamFormat utility.hpp
 * 
 * MIT License
 * 
 * Copyright (c) 2018 Berrysoft
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
#ifndef SF_UTILITY_HPP
#define SF_UTILITY_HPP

#if defined(_MSC_VER) && defined(_UNICODE) && !defined(SF_FORCE_WIDE_IO)
#define SF_FORCE_WIDE_IO 1
#endif // !SF_FORCE_WIDE_IO

#ifndef SF_CHAR_TEMPLATE
#define SF_CHAR_TEMPLATE(name, value)              \
    template <typename Char>                       \
    constexpr Char name{ value };                  \
    template <>                                    \
    constexpr char16_t name<char16_t>{ u##value }; \
    template <>                                    \
    constexpr char32_t name<char32_t>{ U##value }; \
    template <>                                    \
    constexpr wchar_t name<wchar_t>{ L##value };
#endif // !SF_CHAR_TEMPLATE

#endif // !SF_UTILITY_HPP
