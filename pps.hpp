/**
 * @file pps.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 Picture Parameter Set structure.
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

#ifndef _PPS_HPP_
#define _PPS_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"
#include "sps.hpp"
#include "scaling_matrices.hpp"

#include "utilities.hpp"
#include "ioutilities.hpp"
#include "ilog2.hpp"
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

constexpr static inline const char* entropy_coding_mode_flag_to_string(uint32_t entropy_coding_mode_flag)
{
    return entropy_coding_mode_flag ? "cabac" : "cavlc";
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

struct pps : public h264_structure
{
    pps() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    void set_active_sps(const sps* active_sps)
    {
       m_active_sps = active_sps;
    }

    const sps* get_active_sps() const
    {
        return m_active_sps;
    }

    uint32_t pic_parameter_set_id;
    uint32_t seq_parameter_set_id;
    uint32_t entropy_coding_mode_flag;
    uint32_t bottom_field_pic_order_in_frame_present_flag;

    uint32_t num_slice_groups_minus1; /* <= 7 in Baseline and Extended, 0 otheriwse */
    uint32_t slice_group_map_type;
    union {
        struct { /* slice_group_map_type == 0 */
            uint32_t run_length_minus1[8];
        } t0;
        struct { /* slice_group_map_type == 2 */
            uint32_t top_left[8];
            uint32_t bottom_right[8];
        } t1;
        struct { /* slice_group_map_type == 3,4,5 */
            uint32_t slice_group_change_direction_flag;
            uint32_t slice_group_change_rate_minus1;
        } t2;
        struct { /* slice_group_map_type == 6 */
            uint32_t pic_size_in_map_units_minus1;
            uint32_t slice_group_id[256];
        } t3;
    };

    uint32_t num_ref_idx_l0_default_active_minus1;
    uint32_t num_ref_idx_l1_default_active_minus1;
    uint32_t weighted_pred_flag;
    uint32_t weighted_bipred_idc;
     int32_t pic_init_qp_minus26;
     int32_t pic_init_qs_minus26;
     int32_t chroma_qp_index_offset;
    uint32_t deblocking_filter_control_present_flag;
    uint32_t constrained_intra_pred_flag;
    uint32_t redundant_pic_cnt_present_flag;
    uint32_t transform_8x8_mode_flag;

    uint32_t pic_scaling_matrix_present_flag;
    scaling_matrices sm;

    int32_t second_chroma_qp_index_offset;

private:
    const sps* m_active_sps;
};

inline std::string pps::to_string() const
{
    std::ostringstream stream;

    stream << "pps (picture parameter set):" << std::endl;

    stream << P1(pic_parameter_set_id);
    stream << P1(seq_parameter_set_id);
    stream << P1_INFO(entropy_coding_mode_flag, entropy_coding_mode_flag_to_string(entropy_coding_mode_flag));
    stream << P1(bottom_field_pic_order_in_frame_present_flag);

    stream << P1(num_slice_groups_minus1);
    if (num_slice_groups_minus1 > 0) {
        stream << P2(slice_group_map_type);
        if (slice_group_map_type == 0)
            for (std::size_t i = 0; i < TABLE_ELEMENTS(t0.run_length_minus1); ++i)
                stream << P3(t0.run_length_minus1[i]);
        else
        if (slice_group_map_type == 2) {
            for (std::size_t i = 0; i < TABLE_ELEMENTS(t1.top_left); ++i)
                stream << P3(t1.top_left[i]);
            for (std::size_t i = 0; i < TABLE_ELEMENTS(t1.bottom_right); ++i)
                stream << P3(t1.bottom_right[i]);
        }
        else
        if ((slice_group_map_type == 3) ||
            (slice_group_map_type == 4) ||
            (slice_group_map_type == 5)) {
            stream << P3(t2.slice_group_change_direction_flag);
            stream << P3(t2.slice_group_change_rate_minus1);
        }
        else
        if (slice_group_map_type == 6) {
            stream << P3(t3.pic_size_in_map_units_minus1);
            for (uint32_t i = 0; i <= t3.pic_size_in_map_units_minus1; ++i)
                stream << P4(t3.slice_group_id[i]);
        }
    }

    stream << P1(num_ref_idx_l0_default_active_minus1);
    stream << P1(num_ref_idx_l1_default_active_minus1);
    stream << P1(weighted_pred_flag);
    stream << P1(weighted_bipred_idc);
    stream << P1(pic_init_qp_minus26);
    stream << P1(pic_init_qs_minus26);
    stream << P1(chroma_qp_index_offset);
    stream << P1(deblocking_filter_control_present_flag);
    stream << P1(constrained_intra_pred_flag);
    stream << P1(redundant_pic_cnt_present_flag);
    stream << P1(transform_8x8_mode_flag);

    stream << P1(pic_scaling_matrix_present_flag);
    if (pic_scaling_matrix_present_flag)
        stream << sm.to_string();

    stream << P1(second_chroma_qp_index_offset);

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

#endif /* _PPS_HPP_ */
