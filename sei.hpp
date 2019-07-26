/**
 * @file sei.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 Supplemental Enhancement
 * Information structure.
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

#ifndef _SEI_HPP_
#define _SEI_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"
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

constexpr static inline const char* sei_payload_type_to_string(uint32_t sei_payload_type)
{
    const char* str = "invalid 'sei_payload_type' value";

    switch (sei_payload_type) {
        case  0: str = "buffering_period"; break;
        case  1: str = "pic_timing"; break;
        case  2: str = "pan_scan_rect"; break;
        case  3: str = "filler_payload"; break;
        case  4: str = "user_data_registered_itu_t_t35"; break;
        case  5: str = "user_data_unregistered"; break;
        case  6: str = "recovery_point"; break;
        case  7: str = "dec_ref_pic_marking_repetition"; break;
        case  8: str = "spare_pic"; break;
        case  9: str = "scene_info"; break;
        case 10: str = "sub_seq_info"; break;
        case 11: str = "sub_seq_layer_characteristics"; break;
        case 12: str = "sub_seq_characteristics"; break;
        case 13: str = "full_frame_freeze"; break;
        case 14: str = "full_frame_freeze_release"; break;
        case 15: str = "full_frame_snapshot"; break;
        case 16: str = "progressive_refinement_segment_start"; break;
        case 17: str = "progressive_refinement_segment_end"; break;
        case 18: str = "motion_constrained_slice_group_set"; break;
        case 19: str = "film_grain_characteristics"; break;
        case 20: str = "deblocking_filter_display_preference"; break;
        case 21: str = "stereo_video_info"; break;
        case 22: str = "post_filter_hint"; break;
        case 23: str = "tone_mapping_info"; break;
        case 24: str = "scalability_info"; break;
        case 25: str = "sub_pic_scalable_layer"; break;
        case 26: str = "non_required_layer_rep"; break;
        case 27: str = "priority_layer_info"; break;
        case 28: str = "layers_not_present"; break;
        case 29: str = "layer_dependency_change"; break;
        case 30: str = "scalable_nesting"; break;
        case 31: str = "base_layer_temporal_hrd"; break;
        case 32: str = "quality_layer_integrity_check"; break;
        case 33: str = "redundant_pic_property"; break;
        case 34: str = "tl0_dep_rep_index"; break;
        case 35: str = "tl_switching_point"; break;
        case 36: str = "parallel_decoding_info"; break;
        case 37: str = "mvc_scalable_nesting"; break;
        case 38: str = "view_scalability_info"; break;
        case 39: str = "multiview_scene_info"; break;
        case 40: str = "multiview_acquisition_info"; break;
        case 41: str = "non_required_view_component"; break;
        case 42: str = "view_dependency_change"; break;
        case 43: str = "operation_points_not_present"; break;
        case 44: str = "base_view_temporal_hrd"; break;
        case 45: str = "frame_packing_arrangement"; break;
        default:
            str = "reserved sei message";
            break;
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

struct sei : public h264_structure
{
    sei() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    uint32_t payload_type;
    uint32_t payload_size;
    uint8_t  payload_data[16];
};

inline std::string sei::to_string() const
{
    std::ostringstream stream;

    stream << "sei (supplemental enhancement information):" << std::endl;

    stream << P1_INFO(payload_type, sei_payload_type_to_string(payload_type));
    stream << P1(payload_size);

    stream << "    ";
    for (uint32_t i = 0; i < payload_size; ++i)
        stream << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(payload_data[i]) << " ";
    stream << std::endl;

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

#endif /* _SEI_HPP_ */
