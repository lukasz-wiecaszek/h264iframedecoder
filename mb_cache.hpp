/**
 * @file mb_cache.hpp
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

#ifndef _MB_CACHE_HPP_
#define _MB_CACHE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdint>
#include <array>

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

typedef std::array<uint8_t, 5 * 8> mb_cache;

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

/** 
 * mb_cache organization:
 * 
 *    0 1 2 3 4 5 6 7
 * 0
 * 1          o-o o-o
 *             / / /
 * 2          o-o o-o
 * 3          o-o o-o
 *             / / /
 * 4          o-o o-o
 */
const uint8_t mb_cache_idx[16] = 
{
    1 * 8 + 4, 1 * 8 + 5, 
    2 * 8 + 4, 2 * 8 + 5,
    1 * 8 + 6, 1 * 8 + 7, 
    2 * 8 + 6, 2 * 8 + 7,
    
    3 * 8 + 4, 3 * 8 + 5, 
    4 * 8 + 4, 4 * 8 + 5,
    3 * 8 + 6, 3 * 8 + 7, 
    4 * 8 + 6, 4 * 8 + 7,
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

#endif /* _MB_CACHE_HPP_ */
