/**
 * @file mb_info_i.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 intra makroblock constants.
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

#ifndef _MB_INFO_I_HPP_
#define _MB_INFO_I_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "mb_types.hpp"
#include "mb_intra_prediction_modes.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * inline object definitions
\*===========================================================================*/
namespace ymn
{

static const struct {
    int type;
    int pred_mode;
    int cbp_luma;
    int cbp_chroma;
} mb_info_i[26] = {
    {MB_TYPE_INTRA_NxN,  -1,  -1 },
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_VERTICAL,   0x0, 0},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_HORIZONTAL, 0x0, 0},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_DC,         0x0, 0},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_PLANE,      0x0, 0},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_VERTICAL,   0x0, 1},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_HORIZONTAL, 0x0, 1},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_DC,         0x0, 1},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_PLANE,      0x0, 1},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_VERTICAL,   0x0, 2},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_HORIZONTAL, 0x0, 2},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_DC,         0x0, 2},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_PLANE,      0x0, 2},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_VERTICAL,   0xf, 0},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_HORIZONTAL, 0xf, 0},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_DC,         0xf, 0},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_PLANE,      0xf, 0},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_VERTICAL,   0xf, 1},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_HORIZONTAL, 0xf, 1},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_DC,         0xf, 1},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_PLANE,      0xf, 1},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_VERTICAL,   0xf, 2},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_HORIZONTAL, 0xf, 2},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_DC,         0xf, 2},
    {MB_TYPE_INTRA_16x16, MB_INTRA_PRED_LUMA_16x16_PLANE,      0xf, 2},
    {MB_TYPE_INTRA_PCM,  -1, -1 },
};

} /* end of namespace ymn */

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

#endif /* _MB_INFO_I_HPP_ */
