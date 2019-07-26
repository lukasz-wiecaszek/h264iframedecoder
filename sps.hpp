/**
 * @file sps.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 Sequence Parameter Set structure.
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

#ifndef _SPS_HPP_
#define _SPS_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"
#include "scaling_matrices.hpp"
#include "vui_parameters.hpp"

#include "utilities.hpp"
#include "ioutilities.hpp"
#include "istream.hpp"

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

constexpr static inline const char* chroma_format_idc_to_string(uint32_t chroma_format_idc)
{
    const char* str = "invalid 'chroma_format_idc' value";

    switch (chroma_format_idc) {
        case 0: str = "monochrome"; break;
        case 1: str = "4:2:0"; break;
        case 2: str = "4:2:2"; break;
        case 3: str = "4:4:4"; break;
    }

    return str;
}

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

struct sps : public h264_structure
{
    sps() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    uint8_t profile_idc;
    uint8_t constraint_flags;
    uint8_t level_idc;
    uint32_t seq_parameter_set_id;

    uint32_t chroma_format_idc;
    uint32_t separate_colour_plane_flag;
    uint32_t bit_depth_luma_minus8;
    uint32_t bit_depth_chroma_minus8;
    uint32_t qpprime_y_zero_transform_bypass_flag;

    uint32_t seq_scaling_matrix_present_flag;
    scaling_matrices sm;

    uint32_t log2_max_frame_num_minus4;
    uint32_t pic_order_cnt_type;
    union {
         struct { /* pic_order_cnt_type == 0 */
                uint32_t log2_max_pic_order_cnt_lsb_minus4;
         } t0;
         struct { /* pic_order_cnt_type == 1 */
                uint32_t delta_pic_order_always_zero_flag;
                int32_t offset_for_non_ref_pic;
                int32_t offset_for_top_to_bottom_field;
                uint32_t num_ref_frames_in_pic_order_cnt_cycle;
                int32_t offset_for_ref_frame[255];
         } t1;
    };

    uint32_t max_num_ref_frames;
    uint32_t gaps_in_frame_num_value_allowed_flag;
    uint32_t pic_width_in_mbs_minus1;
    uint32_t pic_height_in_map_units_minus1;
    uint32_t frame_mbs_only_flag;
    uint32_t mb_adaptive_frame_field_flag;
    uint32_t direct_8x8_inference_flag;
    uint32_t frame_cropping_flag;
    uint32_t frame_crop_left_offset;
    uint32_t frame_crop_right_offset;
    uint32_t frame_crop_top_offset;
    uint32_t frame_crop_bottom_offset;

    uint32_t vui_parameters_present_flag;
    vui_parameters vui;
};

inline std::string sps::to_string() const
{
    std::ostringstream stream;

    stream << "sps (sequence parameter set):" << std::endl;

    stream << P1(profile_idc);
    stream << P1(constraint_flags);
    stream << P1(level_idc);
    stream << P1(seq_parameter_set_id);

    if (profile_idc == 100 || profile_idc == 110 ||
        profile_idc == 122 || profile_idc == 244 ||
        profile_idc ==  44 || profile_idc ==  83 ||
        profile_idc ==  86 || profile_idc == 118 ||
        profile_idc == 128 || profile_idc == 144) {

        stream << P1_INFO(chroma_format_idc, chroma_format_idc_to_string(chroma_format_idc));
        if (chroma_format_idc == 3)
            stream << P2(separate_colour_plane_flag);

        stream << P1(bit_depth_luma_minus8);
        stream << P1(bit_depth_chroma_minus8);
        stream << P1(qpprime_y_zero_transform_bypass_flag);
        stream << P1(seq_scaling_matrix_present_flag);
        if (seq_scaling_matrix_present_flag)
            stream << sm.to_string();
    }

    stream << P1(log2_max_frame_num_minus4);

    stream << P1(pic_order_cnt_type);
    if (0 == pic_order_cnt_type)
        stream << P2(t0.log2_max_pic_order_cnt_lsb_minus4);
    else
    if (1 == pic_order_cnt_type) {
        stream << P2(t1.delta_pic_order_always_zero_flag);
        stream << P2(t1.offset_for_non_ref_pic);
        stream << P2(t1.offset_for_top_to_bottom_field);
        stream << P2(t1.num_ref_frames_in_pic_order_cnt_cycle);
    }
    else {
        /* do nothing */
    }

    stream << P1(max_num_ref_frames);
    stream << P1(gaps_in_frame_num_value_allowed_flag);
    stream << P1(pic_width_in_mbs_minus1);
    stream << P1(pic_height_in_map_units_minus1);
    stream << P1(frame_mbs_only_flag);
    if (!frame_mbs_only_flag)
        stream << P2(mb_adaptive_frame_field_flag);

    stream << P1(direct_8x8_inference_flag);
    stream << P1(frame_cropping_flag);
    if (frame_cropping_flag) {
        stream << P2(frame_crop_left_offset);
        stream << P2(frame_crop_right_offset);
        stream << P2(frame_crop_top_offset);
        stream << P2(frame_crop_bottom_offset);
    }

    stream << P1(vui_parameters_present_flag);
    if (vui_parameters_present_flag)
        stream << vui.to_string();

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

#endif /* _SPS_HPP_ */
