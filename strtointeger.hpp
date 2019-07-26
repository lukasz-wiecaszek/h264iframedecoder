/**
 * @file strtointeger.hpp
 *
 * Converts string to corresponding integer value.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#ifndef _STRTOINTEGER_HPP_
#define _STRTOINTEGER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <type_traits>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <cerrno>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

enum class strtointeger_conversion_status_e
{
    success,
    invalid_argument,
    out_of_range,
    conversion_error
};

template<typename T>
inline strtointeger_conversion_status_e strtoui(const char* str, T& value)
{
    if (!std::is_integral<T>::value)
        return strtointeger_conversion_status_e::invalid_argument;

    if (!std::is_unsigned<T>::value)
        return strtointeger_conversion_status_e::invalid_argument;

    strtointeger_conversion_status_e status =
        strtointeger_conversion_status_e::conversion_error;

    do {
        unsigned long long ullv;
        char* endptr;

        if (*str == '\0') { /* string is empty */
            status = strtointeger_conversion_status_e::invalid_argument;
            break;
        }

        errno = 0, ullv = std::strtoull(str, &endptr, 0);

        if (*endptr != '\0') {
            status = strtointeger_conversion_status_e::invalid_argument;
            break; /* string is invalid (has unparsed characters) */
        }

        if ((ERANGE == errno) && (ULLONG_MAX == ullv)) {
            status = strtointeger_conversion_status_e::out_of_range;
            break; /* number in string exceeds 'unsigned long long' range */
        }

        if ((0 == ullv) && (errno != 0)) {
            status = strtointeger_conversion_status_e::conversion_error;
            break; /* no valid conversion was performed */
        }

        T max_unsigned_value = static_cast<T>(~0ULL);

        if (ullv > static_cast<unsigned long long>(max_unsigned_value)) {
            status = strtointeger_conversion_status_e::out_of_range;
            break; /* number in string exceeds 'T' range */
        }

        value = static_cast<T>(ullv);
        status = strtointeger_conversion_status_e::success;
    } while (0);

    return status;
}

template<typename T>
inline strtointeger_conversion_status_e strtosi(const char* str, T& value)
{
    if (!std::is_integral<T>::value)
        return strtointeger_conversion_status_e::invalid_argument;

    if (std::is_unsigned<T>::value)
        return strtointeger_conversion_status_e::invalid_argument;

    strtointeger_conversion_status_e status =
        strtointeger_conversion_status_e::conversion_error;

    do {
        long long sllv;
        char* endptr;

        if (*str == '\0') { /* string is empty */
            status = strtointeger_conversion_status_e::invalid_argument;
            break;
        }

        errno = 0, sllv = std::strtoll(str, &endptr, 0);

        if (*endptr != '\0') {
            status = strtointeger_conversion_status_e::invalid_argument;
            break; /* string is invalid (has unparsed characters) */
        }

        if ((ERANGE == errno) && ((LLONG_MIN == sllv) || (LLONG_MAX == sllv))) {
            status = strtointeger_conversion_status_e::out_of_range;
            break; /* number in string exceeds 'long long' range */
        }

        if ((0 == sllv) && (errno != 0)) {
            status = strtointeger_conversion_status_e::conversion_error;
            break; /* no valid conversion was performed */
        }

        typedef typename std::make_unsigned<T>::type unsigned_type;
        unsigned_type max_unsigned_value = static_cast<unsigned_type>(~0ULL);

        T max_signed_value = static_cast<T>(max_unsigned_value >> 1);
        T min_signed_value = static_cast<T>(-max_signed_value - 1);

        if ((sllv > static_cast<long long>(max_signed_value)) ||
            (sllv < static_cast<long long>(min_signed_value))) {
            status = strtointeger_conversion_status_e::out_of_range;
            break; /* number in string exceeds 'T' range */
        }

        value = static_cast<T>(sllv);
        status = strtointeger_conversion_status_e::success;
    } while (0);

    return status;
}

template<typename T>
inline strtointeger_conversion_status_e strtointeger(const char* str, T& value)
{
    if (!std::is_integral<T>::value)
        return strtointeger_conversion_status_e::invalid_argument;

    if (std::is_unsigned<T>::value)
        return strtoui(str, value);
    else
        return strtosi(str, value);
}

} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

#endif /* _STRTOINTEGER_HPP_ */
