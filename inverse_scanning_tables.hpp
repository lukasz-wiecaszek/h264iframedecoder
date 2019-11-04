/**
 * @file inverse_scaning_tables.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 inverse scaning tables
 * as defined in chapter 8.5.6 "Inverse scanning process for 4x4 transform
 * coefficients and scaling lists" and chapter 8.5.7 "Inverse scanning process
 * for 8x8 transform coefficients and scaling lists".
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

#ifndef _INVERSE_SCANING_TABLES_HPP_
#define _INVERSE_SCANING_TABLES_HPP_

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

/* Please see ISO/IEC 14496-10, table 8-13 Specification of mapping of idx to cij for zig-zag and field scan */
inline const uint8_t frame_scan_4x4[16] =
{
    0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
    1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
    1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
    3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};

/* Please see ISO/IEC 14496-10, table 8-13 Specification of mapping of idx to cij for zig-zag and field scan */
inline const uint8_t field_scan_4x4[16] =
{
    0 + 0 * 4, 0 + 1 * 4, 1 + 0 * 4, 0 + 2 * 4,
    0 + 3 * 4, 1 + 1 * 4, 1 + 2 * 4, 1 + 3 * 4,
    2 + 0 * 4, 2 + 1 * 4, 2 + 2 * 4, 2 + 3 * 4,
    3 + 0 * 4, 3 + 1 * 4, 3 + 2 * 4, 3 + 3 * 4,
};

/* Please see ISO/IEC 14496-10, table 8-14 Specification of mapping of idx to cij for 8x8 zig-zag and 8x8 field scan */
inline const uint8_t frame_scan_8x8[64] =
{
    0 + 0 * 8, 1 + 0 * 8, 0 + 1 * 8, 0 + 2 * 8, 1 + 1 * 8, 2 + 0 * 8, 3 + 0 * 8, 2 + 1 * 8,
    1 + 2 * 8, 0 + 3 * 8, 0 + 4 * 8, 1 + 3 * 8, 2 + 2 * 8, 3 + 1 * 8, 4 + 0 * 8, 5 + 0 * 8,
    4 + 1 * 8, 3 + 2 * 8, 2 + 3 * 8, 1 + 4 * 8, 0 + 5 * 8, 0 + 6 * 8, 1 + 5 * 8, 2 + 4 * 8,
    3 + 3 * 8, 4 + 2 * 8, 5 + 1 * 8, 6 + 0 * 8, 7 + 0 * 8, 6 + 1 * 8, 5 + 2 * 8, 4 + 3 * 8,
    3 + 4 * 8, 2 + 5 * 8, 1 + 6 * 8, 0 + 7 * 8, 1 + 7 * 8, 2 + 6 * 8, 3 + 5 * 8, 4 + 4 * 8,
    5 + 3 * 8, 6 + 2 * 8, 7 + 1 * 8, 7 + 2 * 8, 6 + 3 * 8, 5 + 4 * 8, 4 + 5 * 8, 3 + 6 * 8,
    2 + 7 * 8, 3 + 7 * 8, 4 + 6 * 8, 5 + 5 * 8, 6 + 4 * 8, 7 + 3 * 8, 7 + 4 * 8, 6 + 5 * 8,
    5 + 6 * 8, 4 + 7 * 8, 5 + 7 * 8, 6 + 6 * 8, 7 + 5 * 8, 7 + 6 * 8, 6 + 7 * 8, 7 + 7 * 8,
};

/* Please see ISO/IEC 14496-10, table 8-14 Specification of mapping of idx to cij for 8x8 zig-zag and 8x8 field scan */
inline const uint8_t field_scan_8x8[64] =
{
    0 + 0 * 8, 0 + 1 * 8, 0 + 2 * 8, 1 + 0 * 8, 1 + 1 * 8, 0 + 3 * 8, 0 + 4 * 8, 1 + 2 * 8,
    2 + 0 * 8, 1 + 3 * 8, 0 + 5 * 8, 0 + 6 * 8, 0 + 7 * 8, 1 + 4 * 8, 2 + 1 * 8, 3 + 0 * 8,
    2 + 2 * 8, 1 + 5 * 8, 1 + 6 * 8, 1 + 7 * 8, 2 + 3 * 8, 3 + 1 * 8, 4 + 0 * 8, 3 + 2 * 8,
    2 + 4 * 8, 2 + 5 * 8, 2 + 6 * 8, 2 + 7 * 8, 3 + 3 * 8, 4 + 1 * 8, 5 + 0 * 8, 4 + 2 * 8,
    3 + 4 * 8, 3 + 5 * 8, 3 + 6 * 8, 3 + 7 * 8, 4 + 3 * 8, 5 + 1 * 8, 6 + 0 * 8, 5 + 2 * 8,
    4 + 4 * 8, 4 + 5 * 8, 4 + 6 * 8, 4 + 7 * 8, 5 + 3 * 8, 6 + 1 * 8, 6 + 2 * 8, 5 + 4 * 8,
    5 + 5 * 8, 5 + 6 * 8, 5 + 7 * 8, 6 + 3 * 8, 7 + 0 * 8, 7 + 1 * 8, 6 + 4 * 8, 6 + 5 * 8,
    6 + 6 * 8, 6 + 7 * 8, 7 + 2 * 8, 7 + 3 * 8, 7 + 4 * 8, 7 + 5 * 8, 7 + 6 * 8, 7 + 7 * 8,
};

inline const uint8_t scan_table_chroma_dc[4] =
{
    0, 1, 2, 3
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

#endif /* _INVERSE_SCANING_TABLES_HPP_ */
