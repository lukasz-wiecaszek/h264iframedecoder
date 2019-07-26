/**
 * @file mb_types.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 makroblock types.
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

#ifndef _MB_TYPES_HPP_
#define _MB_TYPES_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define MB_TYPE_INTRA_4x4      0x00000001
#define MB_TYPE_INTRA_8x8      0x00000002
#define MB_TYPE_INTRA_NxN     (MB_TYPE_INTRA_4x4 | MB_TYPE_INTRA_8x8)
#define MB_TYPE_INTRA_16x16    0x00000004
#define MB_TYPE_INTRA_PCM      0x00000008
#define MB_TYPE_INTRA         (MB_TYPE_INTRA_NxN | MB_TYPE_INTRA_16x16 | MB_TYPE_INTRA_PCM)

#define MB_TYPE_INTER_16x16    0x00000010
#define MB_TYPE_INTER_16x8     0x00000020
#define MB_TYPE_INTER_8x16     0x00000040
#define MB_TYPE_INTER_8x8      0x00000080
#define MB_TYPE_INTER         (MB_TYPE_INTER_16x16 | MB_TYPE_INTER_16x8 | MB_TYPE_INTER_8x16 | MB_TYPE_INTER_8x8)

#define MB_TYPE_INTERLACED     0x00000100
#define MB_TYPE_DIRECT2        0x00000200
#define MB_TYPE_ACPRED         0x00000400
#define MB_TYPE_GMC            0x00000800
#define MB_TYPE_SKIP           0x00001000
#define MB_TYPE_P0L0           0x00002000
#define MB_TYPE_P1L0           0x00004000
#define MB_TYPE_P0L1           0x00008000
#define MB_TYPE_P1L1           0x00010000
#define MB_TYPE_L0            (MB_TYPE_P0L0 | MB_TYPE_P1L0)
#define MB_TYPE_L1            (MB_TYPE_P0L1 | MB_TYPE_P1L1)
#define MB_TYPE_L0L1          (MB_TYPE_L0   | MB_TYPE_L1)
#define MB_TYPE_QUANT          0x00020000
#define MB_TYPE_CBP            0x00040000
#define MB_TYPE_8x8DCT         0x00080000

#define MB_IS_INTRA_4x4(x)      (((x) & MB_TYPE_INTRA_NxN) == MB_TYPE_INTRA_4x4)
#define MB_IS_INTRA_8x8(x)      (((x) & MB_TYPE_INTRA_NxN) == MB_TYPE_INTRA_8x8)
#define MB_IS_INTRA_NxN(x)       ((x) & MB_TYPE_INTRA_NxN)
#define MB_IS_INTRA_16x16(x)     ((x) & MB_TYPE_INTRA_16x16)
#define MB_IS_INTRA_PCM(x)       ((x) & MB_TYPE_INTRA_PCM)
#define MB_IS_INTRA(x)           ((x) & MB_TYPE_INTRA)

#define MB_IS_INTER_16X16(x)     ((x) & MB_TYPE_INTER_16x16)
#define MB_IS_INTER_16X8(x)      ((x) & MB_TYPE_INTER_16x8)
#define MB_IS_INTER_8X16(x)      ((x) & MB_TYPE_INTER_8x16)
#define MB_IS_INTER_8X8(x)       ((x) & MB_TYPE_INTER_8x8)
#define MB_IS_INTER_SUB_8X8(x)   ((x) & MB_TYPE_INTER_16x16)
#define MB_IS_INTER_SUB_8X4(x)   ((x) & MB_TYPE_INTER_16x8)
#define MB_IS_INTER_SUB_4X8(x)   ((x) & MB_TYPE_INTER_8x16)
#define MB_IS_INTER_SUB_4X4(x)   ((x) & MB_TYPE_INTER_8x8)
#define MB_IS_INTER(x)           ((x) & MB_TYPE_INTER)

#define MB_IS_SKIP(x)            ((x) & MB_TYPE_SKIP)
#define MB_IS_INTERLACED(x)      ((x) & MB_TYPE_INTERLACED)
#define MB_IS_DIRECT(x)          ((x) & MB_TYPE_DIRECT2)
#define MB_IS_GMC(x)             ((x) & MB_TYPE_GMC)
#define MB_IS_ACPRED(x)          ((x) & MB_TYPE_ACPRED)
#define MB_IS_QUANT(x)           ((x) & MB_TYPE_QUANT)
#define MB_IS_DIR(x, part, list) ((x) & (MB_TYPE_P0L0<<((part)+2*(list))))
#define MB_USES_LIST(x, list)    ((x) & ((MB_TYPE_P0L0|MB_TYPE_P1L0)<<(2*(list))))
#define MB_HAS_CBP(x)            ((x) & MB_TYPE_CBP)
#define MB_IS_8x8DCT(x)          ((x) & MB_TYPE_8x8DCT)

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

#endif /* _MB_TYPES_HPP_ */
