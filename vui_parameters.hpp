/**
 * @file vui_parameters.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 VUI
 * (Video Usability Information) Parameters structure (Annex E).
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

#ifndef _VUI_PARAMETERS_HPP_
#define _VUI_PARAMETERS_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"
#include "aspect_ratio_idc.hpp"
#include "hrd_parameters.hpp"
#include "ioutilities.hpp"
#include "utilities.hpp"

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

constexpr static inline const char* aspect_ratio_idc_to_string(uint32_t aspect_ratio_idc)
{
    const char* str = "invalid 'aspect_ratio_idc' value";

    if (aspect_ratio_idc <= 255) {
        switch (aspect_ratio_idc) {
            case   0: str = "unspecified"; break;
            case   1: str = "1:1 (square)"; break;
            case   2: str = "12:11"; break;
            case   3: str = "10:11"; break;
            case   4: str = "16:11"; break;
            case   5: str = "40:33"; break;
            case   6: str = "24:11"; break;
            case   7: str = "20:11"; break;
            case   8: str = "32:11"; break;
            case   9: str = "80:33"; break;
            case  10: str = "18:11"; break;
            case  11: str = "15:11"; break;
            case  12: str = "64:33"; break;
            case  13: str = "160:99"; break;
            case  14: str = "4:3"; break;
            case  15: str = "3:2"; break;
            case  16: str = "2:1"; break;
            case 255: str = "extended sample aspect ratio"; break;
            default:
                str = "reserved";
                break;
        }
    }

    return str;
}

constexpr static inline const char* video_format_to_string(uint32_t video_format)
{
    const char* str = "invalid 'video_format' value";

    if (video_format <= 7) {
        switch (video_format) {
            case 0: str = "component"; break;
            case 1: str = "pal"; break;
            case 2: str = "ntsc"; break;
            case 3: str = "secam"; break;
            case 4: str = "mac"; break;
            case 5: str = "unspecified video format"; break;
            default:
                str = "reserved";
                break;
        }
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

struct vui_parameters : public h264_structure
{
    vui_parameters() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    uint32_t aspect_ratio_info_present_flag;
        uint32_t aspect_ratio_idc;
            uint32_t sar_width;
            uint32_t sar_height;
    uint32_t overscan_info_present_flag;
        uint32_t overscan_appropriate_flag;
    uint32_t video_signal_type_present_flag;
        uint32_t video_format;
        uint32_t video_full_range_flag;
        uint32_t colour_description_present_flag;
            uint32_t colour_primaries;
            uint32_t transfer_characteristics;
            uint32_t matrix_coefficients;
    uint32_t chroma_loc_info_present_flag;
        uint32_t chroma_sample_loc_type_top_field;
        uint32_t chroma_sample_loc_type_bottom_field;
    uint32_t timing_info_present_flag;
        uint32_t num_units_in_tick;
        uint32_t time_scale;
        uint32_t fixed_frame_rate_flag;

    uint32_t nal_hrd_parameters_present_flag;
    hrd_parameters nal_hrd;

    uint32_t vcl_hrd_parameters_present_flag;
    hrd_parameters vcl_hrd;

    uint32_t low_delay_hrd_flag;

    uint32_t pic_struct_present_flag;
    uint32_t bitstream_restriction_flag;
        uint32_t motion_vectors_over_pic_boundaries_flag;
        uint32_t max_bytes_per_pic_denom;
        uint32_t max_bits_per_mb_denom;
        uint32_t log2_max_mv_length_horizontal;
        uint32_t log2_max_mv_length_vertical;
        uint32_t max_num_reorder_frames;
        uint32_t max_dec_frame_buffering;
};

inline std::string vui_parameters::to_string() const
{
    std::ostringstream stream;

    stream << "  vui (video usability information) parameters:" << std::endl;
    stream << P2(aspect_ratio_info_present_flag);
    if (aspect_ratio_info_present_flag) {
        stream << P3_INFO(aspect_ratio_idc, aspect_ratio_idc_to_string(aspect_ratio_idc));
        if (255 == aspect_ratio_idc) {
            stream << P4(sar_width);
            stream << P4(sar_height);
        }
    }

    stream << P2(overscan_info_present_flag);
    if (overscan_info_present_flag)
        stream << P3(overscan_appropriate_flag);

    stream << P2(video_signal_type_present_flag);
    if (video_signal_type_present_flag) {
        stream << P3_INFO(video_format, video_format_to_string(video_format));
        stream << P3(video_full_range_flag);
        stream << P3(colour_description_present_flag);
        if (colour_description_present_flag) {
            stream << P4(colour_primaries);
            stream << P4(transfer_characteristics);
            stream << P4(matrix_coefficients);
        }
    }

    stream << P2(chroma_loc_info_present_flag);
    if (chroma_loc_info_present_flag) {
        stream << P3(chroma_sample_loc_type_top_field);
        stream << P3(chroma_sample_loc_type_bottom_field);
    }

    stream << P2(timing_info_present_flag);
    if (timing_info_present_flag) {
        stream << P3(num_units_in_tick);
        stream << P3(time_scale);
        stream << P3(fixed_frame_rate_flag);
    }

    stream << P2(nal_hrd_parameters_present_flag);
    if (nal_hrd_parameters_present_flag)
        stream << nal_hrd.to_string();

    stream << P2(vcl_hrd_parameters_present_flag);
    if (vcl_hrd_parameters_present_flag)
        stream << vcl_hrd.to_string();

    if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
        stream << P3(low_delay_hrd_flag);

    stream << P2(pic_struct_present_flag);
    stream << P2(bitstream_restriction_flag);
    if (bitstream_restriction_flag) {
        stream << P3(motion_vectors_over_pic_boundaries_flag);
        stream << P3(max_bytes_per_pic_denom);
        stream << P3(max_bits_per_mb_denom);
        stream << P3(log2_max_mv_length_horizontal);
        stream << P3(log2_max_mv_length_vertical);
        stream << P3(max_num_reorder_frames);
        stream << P3(max_dec_frame_buffering);
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

#endif /* _VUI_PARAMETERS_HPP_ */
