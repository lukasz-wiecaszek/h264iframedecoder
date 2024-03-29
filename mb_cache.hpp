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
#include "utilities.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define MB_CACHE_ROWS    5
#define MB_CACHE_COLUMNS 8

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

using mb_cache = std::array<uint8_t, MB_CACHE_ROWS * MB_CACHE_COLUMNS>;

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
inline const uint8_t mb_cache_idx[16] =
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

inline void mb_cache_fill_rectangle_2x2(mb_cache& cache, int offset, uint8_t pattern)
{
    const uint16_t v = pattern * 0x0101U;
    uint8_t *p = &cache[offset];

    DEREFERENCE(uint16_t, p + 0 * MB_CACHE_COLUMNS) = v;
    DEREFERENCE(uint16_t, p + 1 * MB_CACHE_COLUMNS) = v;
}

inline void mb_cache_fill_rectangle_4x4(mb_cache& cache, int offset, uint8_t pattern)
{
    const uint32_t v = pattern * 0x01010101U;
    uint8_t *p = &cache[offset];

    DEREFERENCE(uint32_t, p + 0 * MB_CACHE_COLUMNS) = v;
    DEREFERENCE(uint32_t, p + 1 * MB_CACHE_COLUMNS) = v;
    DEREFERENCE(uint32_t, p + 2 * MB_CACHE_COLUMNS) = v;
    DEREFERENCE(uint32_t, p + 3 * MB_CACHE_COLUMNS) = v;
}

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
