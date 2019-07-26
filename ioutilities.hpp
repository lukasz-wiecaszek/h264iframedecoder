/**
 * @file ioutilities.hpp
 *
 * Set of miscellaneous io utility routines.
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

#ifndef _IOUTILITIES_HPP_
#define _IOUTILITIES_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <iostream>
#include <iomanip>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define AS_INTEGER(value) \
    ((sizeof(value) < sizeof(int)) ? static_cast<int>(value) : (value))

#define HEX(value) "0x" << std::hex << AS_INTEGER(value)
#define DEC(value)         std::dec << AS_INTEGER(value)

#define HEXDEC(value) HEX(value) << " (" << DEC(value) << ")"

#define L(item, indentation) \
    std::setw(indentation) << "" << \
    std::setw(48 - indentation) << std::left << #item << ": " << HEXDEC(item)

#define P0(item) L(item, 0) << std::endl
#define P0_INFO(item, info) L(item, 0) << ": " << info << std::endl

#define P1(item) L(item, 2) << std::endl
#define P1_INFO(item, info) L(item, 2) << ": " << info << std::endl

#define P2(item) L(item, 4) << std::endl
#define P2_INFO(item, info) L(item, 4) << ": " << info << std::endl

#define P3(item) L(item, 6) << std::endl
#define P3_INFO(item, info) L(item, 6) << ": " << info << std::endl

#define P4(item) L(item, 8) << std::endl
#define P4_INFO(item, info) L(item, 8) << ": " << info << std::endl

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

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

#endif /* _IOUTILITIES_HPP_ */
