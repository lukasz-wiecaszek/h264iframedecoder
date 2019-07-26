/**
 * @file utilities.hpp
 *
 * Set of miscellaneous utility routines.
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

#ifndef _UTILITIES_HPP_
#define _UTILITIES_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <climits>
#include <iomanip>
#include <cerrno>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define UNUSED(expr) do {(void)(expr);} while(0)

#define TABLE_ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

#define CHK_EXPR(expr, ostream) chk_expr(#expr, expr, ostream)

#define CHK_MIN(item, min, ostream) \
    chk_min(#item, item, min, ostream)

#define CHK_STRUCT_MIN(struct, item, min, ostream) \
    chk_min(#struct"."#item, struct.item, min, ostream)

#define CHK_MAX(item, max, ostream) \
    chk_max(#item, item, max, ostream)

#define CHK_STRUCT_MAX(struct, item, max, ostream) \
    chk_max(#struct"."#item, struct.item, max, ostream)

#define CHK_RANGE(item, min, max, ostream) \
    chk_range(#item, item, min, max, ostream)

#define CHK_STRUCT_RANGE(struct, item, min, max, ostream) \
    chk_range(#struct"."#item, struct.item, min, max, ostream)

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

inline bool chk_expr(const char* expr, bool status, std::ostream& ostream)
{
    if (!status)
        if (ostream)
            ostream << "error: " << expr << " failed" << std::endl;

    return status;
}

template<typename T>
inline bool chk_min(const char* item_name, T item_value, T min, std::ostream& ostream)
{
    if (item_value >= min)
        return true;

    if (ostream) {
        ostream << "error: value of '" << item_name << "' " << HEXDEC(item_value);
        ostream << " shall not be less than " << HEXDEC(min);
        ostream << std::endl;
    }

    return false;
}

template<typename T>
inline bool chk_max(const char* item_name, T item_value, T max, std::ostream& ostream)
{
    if (item_value <= max)
        return true;

    if (ostream) {
        ostream << "error: value of '" << item_name << "' " << HEXDEC(item_value);
        ostream << " shall not be greater than " << HEXDEC(max);
        ostream << std::endl;
    }

    return false;
}

template<typename T>
inline bool chk_range(const char* item_name, T item_value, T min, T max, std::ostream& ostream)
{
    if ((item_value >= min) && (item_value <= max))
        return true;

    if (ostream) {
        ostream << "error: value of '" << item_name << "' " << HEXDEC(item_value);
        ostream << " shall be in range [" << HEXDEC(min) << ", " << HEXDEC(max) << "]";
        ostream << std::endl;
    }

    return false;
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

#endif /* _UTILITIES_HPP_ */
