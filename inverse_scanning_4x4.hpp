/**
 * @file inverse_scanning_4x4.hpp
 *
 * ...
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

#ifndef _INVERSE_SCANNING_4x4_HPP_
#define _INVERSE_SCANNING_4x4_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdint>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

/* Please see ISO/IEC 14496-10, figure 6-10 – Scan for 4x4 luma blocks */
const uint8_t inverse_scanning_4x4[16] =
{
    0 * 4 + 0, 0 * 4 + 1, 1 * 4 + 0, 1 * 4 + 1,
    0 * 4 + 2, 0 * 4 + 3, 1 * 4 + 2, 1 * 4 + 3,
    2 * 4 + 0, 2 * 4 + 1, 3 * 4 + 0, 3 * 4 + 1,
    2 * 4 + 2, 2 * 4 + 3, 3 * 4 + 2, 3 * 4 + 3,
};

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
} /* end of namespace ymn */

#endif /* _INVERSE_SCANNING_4x4_HPP_ */
