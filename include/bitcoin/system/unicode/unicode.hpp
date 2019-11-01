/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_UNICODE_HPP
#define LIBBITCOIN_SYSTEM_UNICODE_HPP

#include <cstddef>
#include <iostream>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>

// Regarding Unicode design for Windows:
//
// Windows and other environments, such as Java, that supported Unicode prior
// to the advent of utf8 utilize 16 bit characters. These are typically encoded
// as wchar_t in C++. Unicode no longer fits in 16 bits and as such these
// implementations now require variable length character encoding just as utf8.
//
// Libbitcoin embraces the "utf8 everywhere" design: http://utf8everywhere.org
// The objective is to use utf8 as the canonical string encoding, pushing
// wchar_t translation to the edge (stdio, argv, O/S and external API calls).
// The macro BC_USE_LIBBITCOIN_MAIN does most of the heavy lifting to ensure
// that stdio and argv is configured for utf8. The 'to_utf' functions are
// provided for API translations.

// Regarding Unicode source files in VC++ builds:
//
// Don't use #pragma execution_character_set("utf-8") in Windows.
// This instruction causes sources to be explicitly interpreted as UTF8.
// However this technique improperly encodes literals ("フ" for example).
// Instead use non-BOM UTF encoded files. To do this use "save as..."
// utf8 without a "signature" (BOM) in the Visual Studio save dialog.
// This is the typical file format for C++ sources in other environments.

// Regarding Unicode in console applications:
//
// BC_USE_LIBBITCOIN_MAIN should be declared prior to bc::system::main() in a
// console application. This enables Unicode argument and environment
// processing in Windows. This macro implements main() and forwards to
// bc::system::main(), which should be implemented as if it was main() with
// the expectation that argv is utf8.
//
// Do not use std::cout|std::cerr|std::cin (aborts on assertion):
// std::cout << "text";
// std::cerr << "text";
// std::string text;
// std::cin >> text;
//
// Do not use the L qualifier when source is UTF-8 w/out BOM (mangles output):
// const auto utf16 = L"acción.кошка.日本国";
// std::wcout << utf16;

// Regarding use of boost:
//
// When working with boost and utf8 narrow characters on Windows the thread
// must be configured for utf8. When working with boost::filesystem::path the
// static path object must be imbued with the utf8 locale or paths will be
// incorrectly translated.

#define BC_LOCALE_BACKEND "icu"
#define BC_LOCALE_UTF8 "en_US.UTF8"

#ifdef _MSC_VER
    #include <cstdlib>
    #include <locale>
    #include <boost/filesystem.hpp>
    #include <boost/locale.hpp>
    #include <windows.h>
    #define BC_USE_LIBBITCOIN_MAIN \
        namespace libbitcoin { \
        namespace system { \
        std::istream& cin = cin_stream(); \
        std::ostream& cout = cout_stream(); \
        std::ostream& cerr = cerr_stream(); \
        \
        int main(int argc, char* argv[]); \
        } \
        } \
        \
        int wmain(int argc, wchar_t* argv[]) \
        { \
            using namespace libbitcoin::system; \
            boost::locale::generator locale; \
            std::locale::global(locale(BC_LOCALE_UTF8)); \
            boost::filesystem::path::imbue(std::locale()); \
            \
            auto environment = environ; \
            environ = bc::system::allocate_environment(_wenviron); \
            \
            auto arguments = bc::system::allocate_environment(argc, argv); \
            const auto result = libbitcoin::system::main(argc, arguments); \
            \
            bc::system::free_environment(arguments); \
            bc::system::free_environment(environ); \
            environ = environment; \
            return result; \
        }
#else
    #define BC_USE_LIBBITCOIN_MAIN \
        namespace libbitcoin { \
        namespace system { \
        std::istream& cin = std::cin; \
        std::ostream& cout = std::cout; \
        std::ostream& cerr = std::cerr; \
        int main(int argc, char* argv[]); \
        } \
        } \
        \
        int main(int argc, char* argv[]) \
        { \
            return libbitcoin::system::main(argc, argv); \
        }
#endif

namespace libbitcoin {
namespace system {

/**
 * Use bc::system::cin in place of std::cin, see BC_USE_LIBBITCOIN_MAIN.
 */
std::istream& cin_stream();

/**
 * Use bc::system::cout in place of std::cout, see BC_USE_LIBBITCOIN_MAIN.
 */
std::ostream& cout_stream();

/**
 * Use bc::system::cerr in place of std::cerr, see BC_USE_LIBBITCOIN_MAIN.
 */
std::ostream& cerr_stream();

#ifdef WITH_ICU

/**
 * Normalize a string value using nfc normalization.
 * Failure is indicated by empty string return for non-empty value.
 * This function requires the ICU dependency.
 * @param[in]  value  The value to normalize.
 * @return            The normalized value.
 */
BC_API std::string to_normal_nfc_form(const std::string& value);

/**
 * Normalize a string value using nfkd normalization.
 * Failure is indicated by empty string return for non-empty value.
 * This function requires the ICU dependency.
 * @param[in]  value  The value to normalize.
 * @return            The normalized value.
 */
BC_API std::string to_normal_nfkd_form(const std::string& value);

/**
 * Lowercase a string value with locale awareness where possible.
 * This function requires the ICU dependency.
 * @param[in]  value  The value to lowercase.
 * @return            The lowercased value.
 */
BC_API std::string to_lower(const std::string& value);

#endif

/**
 * Free an environment.
 * @param[in]  environment  The environment to free.
 */
BC_API void free_environment(char* environment[]);

/**
 * Convert wide environment to allocated utf8 environment (caller must free).
 * @param[in]  environment  The wide environment variables buffer.
 * @return                  A buffer holding the narrow version of environment.
 */
BC_API char** allocate_environment(wchar_t* environment[]);

/**
 * Convert wide arguments to allocated utf8 arguments (caller must free).
 * @param[in]  argc  The number of elements in argv.
 * @param[in]  argv  The wide command line arguments.
 * @return           A buffer holding the narrow version of argv.
 */
BC_API char** allocate_environment(int argc, wchar_t* argv[]);

/**
 * Convert a wide (presumed UTF16) array to wide (UTF8/char).
 * @param[in]  out        The converted narrow array.
 * @param[in]  out_bytes  The allocated number of bytes in 'out'.
 * @param[in]  in         The wide character array to convert.
 * @param[in]  in_chars   The number of 'in' wide characters to convert.
 * @return                The number of bytes converted.
 */
BC_API size_t to_utf8(char out[], size_t out_bytes, const wchar_t in[],
    size_t in_chars);

/**
 * Convert a wide (presumed UTF16) string to narrow (UTF8/char).
 * @param[in]  wide  The utf16 string to convert.
 * @return           The resulting utf8 string.
 */
BC_API std::string to_utf8(const std::wstring& wide);

/**
 * Convert a narrow (presumed UTF8) array to wide (UTF16/wchar_t).
 * This is designed for buffering, where the narrow array may have been
 * truncated in the middle of a multiple byte character. The terminating
 * offset is returned in the 'unread' out parameter.
 * @param[in]  out        The converted wide array.
 * @param[in]  out_chars  The allocated number of wide characters in 'out'.
 * @param[in]  in         The narrow array to convert.
 * @param[in]  in_bytes   The number of 'in' bytes to convert.
 * @param[in]  truncated  The number of 'in' bytes [0..3] that were truncated.
 * @return                The number of characters converted.
 */
BC_API size_t to_utf16(wchar_t out[], size_t out_chars, const char in[],
    size_t in_bytes, uint8_t& truncated);

/**
 * Convert a narrow (presumed UTF8) string to wide (UTF16/wchar_t).
 * @param[in]  narrow  The utf8 string to convert.
 * @return             The resulting utf16 string.
 */
BC_API std::wstring to_utf16(const std::string& narrow);

/**
 * Initialize windows to use UTF8 for stdio. This cannot be uninitialized and
 * once set bc stdio must be used in place of std stdio.
 */
BC_API void set_utf8_stdio();

/**
 * Initialize windows to use UTF8 for stdin. This cannot be uninitialized and
 * once set bc::system::cin must be used in place of std::cin.
 */
BC_API void set_utf8_stdin();

/**
 * Initialize windows to use UTF8 for stdout. This cannot be uninitialized and
 * once set bc::system::cout must be used in place of std::cout.
 */
BC_API void set_utf8_stdout();

/**
 * Initialize windows to use UTF8 for stderr. This cannot be uninitialized and
 * once set bc::system::cerr must be used in place of std::cerr.
 */
BC_API void set_utf8_stderr();

/**
 * Initialize windows to use binary for stdin. This cannot be uninitialized.
 */
BC_API void set_binary_stdin();

/**
 * Initialize windows to use binary for stdout. This cannot be uninitialized.
 */
BC_API void set_binary_stdout();

} // namespace system
} // namespace libbitcoin

#endif
