/**
 * @file mb.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 ...
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

#ifndef _MB_HPP_
#define _MB_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdint>
#include <cstdio>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "colour_component.hpp"
#include "mb_types.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

struct mb
{
    int x, y, pos;
    int slice_num;

    /* macroblock type */
    int type;

    /* neighbors */
    struct mb *A;
    struct mb *B;
    struct mb *C;
    struct mb *D;
    struct mb *left;
    struct mb *left_pair[2];
    struct mb *top;

    /* prediction stuff */
    union {
        uint8_t m4x4[16];
        uint8_t m8x8[4];
        uint8_t m16x16;
    } intra_luma_pred_mode;
    uint8_t intra_chroma_pred_mode;

    /* coded block pattern */
    int cbp_luma;
    int cbp_chroma;

    /* luma quantization parameter */
    int luma_qp;

    /* Number of non-zero AC coefficiets in 4x4 blocks in Y,Cb and Cr components.
     Last three indexes store number of non-zero DC coefficients in Y,Cb and Cr
     components respectively. */
    uint8_t non_zero_count[16 * COLOUR_COMPONENTS_MAX + COLOUR_COMPONENTS_MAX];

    std::string to_string() const;

    operator std::string () const
    {
        return to_string();
    }
};

inline std::string mb::to_string() const
{
    char buf[4096];
    int status, n = 0;

    status = snprintf(&buf[n], sizeof(buf) - n,
        "[%d:%2d,%2d], neighbours: [%d,%d,%d,%d], left: %d, top: %d, type: 0x%08x, cbp_luma: 0x%02x, cbp_chroma: %d, luma_qp: %d\n",
        slice_num, x, y,
        A != nullptr, B != nullptr, C != nullptr, D != nullptr, left != nullptr, top != nullptr,
        type, cbp_luma, cbp_chroma, luma_qp);
    assert((status > 0) && (static_cast<size_t>(status) < (sizeof(buf) - n)));
    n += status;

    if (MB_IS_INTRA_4x4(type)) {
        const uint8_t* p = intra_luma_pred_mode.m4x4;
        status = snprintf(&buf[n], sizeof(buf) - n,
            "[%d:%2d,%2d], intra pred luma: [%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d], chroma: %d\n",
            slice_num, x, y,
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15], intra_chroma_pred_mode);
        assert((status > 0) && (static_cast<size_t>(status) < (sizeof(buf) - n)));
        n += status;
    }
    else
    if (MB_IS_INTRA_8x8(type)) {
        const uint8_t* p = intra_luma_pred_mode.m8x8;
        status = snprintf(&buf[n], sizeof(buf) - n,
            "[%d:%2d,%2d], intra pred luma: [%d %d %d %d], chroma: %d\n",
            slice_num, x, y,
            p[0], p[1], p[2], p[3], intra_chroma_pred_mode);
        assert((status > 0) && (static_cast<size_t>(status) < (sizeof(buf) - n)));
        n += status;
    }
    else
    if (MB_IS_INTRA_16x16(type)) {
        status = snprintf(&buf[n], sizeof(buf) - n,
            "[%d:%2d,%2d], intra pred luma: %d, chroma: %d\n",
            slice_num, x, y,
            intra_luma_pred_mode.m16x16, intra_chroma_pred_mode);
        assert((status > 0) && (static_cast<size_t>(status) < (sizeof(buf) - n)));
        n += status;
    }
    else {
        /* do nothing */
    }

    return std::string(buf);
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

#endif /* _MB_HPP_ */
