/**
 * @file pred_weight_table.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 Prediction Weight Table
 * structure.
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

#ifndef _PRED_WEIGHT_TABLE_HPP_
#define _PRED_WEIGHT_TABLE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"
#include "slice_type.hpp"
#include "ioutilities.hpp"

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

struct pred_weight_table : public h264_structure
{
    pred_weight_table() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    struct lx
    {
        uint32_t luma_weight_lx_flag[32];
        int32_t luma_weight_lx[32];
        int32_t luma_offset_lx[32];
        uint32_t chroma_weight_lx_flag[32];
        int32_t chroma_weight_lx[32][2];
        int32_t chroma_offset_lx[32][2];
    };

    slice_type_e slice_type;
    bool chroma_components_present;

    uint32_t luma_log2_weight_denom;
    uint32_t chroma_log2_weight_denom;

    uint32_t num_ref_idx_l0_active_minus1;
    lx l0;

    uint32_t num_ref_idx_l1_active_minus1;
    lx l1;
};

inline std::string pred_weight_table::to_string() const
{
    std::ostringstream stream;

    stream << "  prediction weight table:" << std::endl;

    stream << P2(luma_log2_weight_denom);
    if (chroma_components_present)
        stream << P2(chroma_log2_weight_denom);

    stream << "  prediction weight table l0:" << std::endl;

    for (uint32_t i = 0; i <= num_ref_idx_l0_active_minus1; ++i) {

        stream << P2(l0.luma_weight_lx_flag[i]);
        if (l0.luma_weight_lx_flag[i]) {
            stream << P3(l0.luma_weight_lx[i]);
            stream << P3(l0.luma_offset_lx[i]);
        }

        if (chroma_components_present) {
            stream << P2(l0.chroma_weight_lx_flag[i]);
            if (l0.chroma_weight_lx_flag[i]) {
                stream << P3(l0.chroma_weight_lx[i][0]);
                stream << P3(l0.chroma_offset_lx[i][0]);
                stream << P3(l0.chroma_weight_lx[i][1]);
                stream << P3(l0.chroma_offset_lx[i][1]);
            }
        }
    }

    if (slice_type == slice_type_e::B) {
        stream << "  prediction weight table l1:" << std::endl;

        for (uint32_t i = 0; i <= num_ref_idx_l1_active_minus1; ++i) {

            stream << P2(l1.luma_weight_lx_flag[i]);
            if (l1.luma_weight_lx_flag[i]) {
                stream << P3(l1.luma_weight_lx[i]);
                stream << P3(l1.luma_offset_lx[i]);
            }

            if (chroma_components_present) {
                stream << P2(l1.chroma_weight_lx_flag[i]);
                if (l0.chroma_weight_lx_flag[i]) {
                    stream << P3(l1.chroma_weight_lx[i][0]);
                    stream << P3(l1.chroma_offset_lx[i][0]);
                    stream << P3(l1.chroma_weight_lx[i][1]);
                    stream << P3(l1.chroma_offset_lx[i][1]);
                }
            }
        }
    }

    return stream.str();
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

#endif /* _PRED_WEIGHT_TABLE_HPP_ */
