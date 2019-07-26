/**
 * @file slice_header.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 Slice Header structure.
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

#ifndef _SLICE_HEADER_HPP_
#define _SLICE_HEADER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"
#include "pps.hpp"
#include "nal_unit_type.hpp"
#include "slice_type.hpp"
#include "ref_pic_list_modification.hpp"
#include "pred_weight_table.hpp"
#include "dec_ref_pic_marking.hpp"

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

struct slice_header : public h264_structure
{
    slice_header() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    void set_active_pps(const pps* active_pps)
    {
       m_active_pps = active_pps;
    }

    const pps* get_active_pps() const
    {
        return m_active_pps;
    }

    uint32_t nal_ref_idc;
    uint32_t nal_unit_type;

    uint32_t first_mb_in_slice;
    uint32_t slice_type;
    uint32_t pic_parameter_set_id;
    uint32_t colour_plane_id;
    uint32_t frame_num;
    uint32_t field_pic_flag;
    uint32_t bottom_field_flag;
    uint32_t idr_pic_id;
    uint32_t pic_order_cnt_lsb;
     int32_t delta_pic_order_cnt_bottom;
     int32_t delta_pic_order_cnt[2];
    uint32_t redundant_pic_cnt;
    uint32_t direct_spatial_mv_pred_flag;
    uint32_t num_ref_idx_active_override_flag;
    uint32_t num_ref_idx_l0_active_minus1;
    uint32_t num_ref_idx_l1_active_minus1;

    ref_pic_list_modification rplm_l0;
    ref_pic_list_modification rplm_l1;

    pred_weight_table pwt;

    dec_ref_pic_marking_idr drpm_idr;
    dec_ref_pic_marking_nonidr drpm_nonidr;

    uint32_t cabac_init_idc;
     int32_t slice_qp_delta;
    uint32_t sp_for_switch_flag;
     int32_t slice_qs_delta;
    uint32_t disable_deblocking_filter_idc;
     int32_t slice_alpha_c0_offset_div2;
     int32_t slice_beta_offset_div2;
    uint32_t slice_group_change_cycle;

private:
    const pps* m_active_pps;
};

inline std::string slice_header::to_string() const
{
    std::ostringstream stream;
    const sps* sps;
    const pps* pps;

    pps = this->get_active_pps();
    sps = pps->get_active_sps();

    stream << "slice header:" << std::endl;

    stream << P1(nal_ref_idc);
    stream << P1_INFO(nal_unit_type, ymn::to_string(static_cast<nal_unit_type_e>(nal_unit_type)));

    stream << P1(first_mb_in_slice);
    stream << P1_INFO(slice_type, ymn::h264::to_string(static_cast<slice_type_e>(slice_type % 5)));
    stream << P1(pic_parameter_set_id);

    if (sps->separate_colour_plane_flag)
        stream << P1(colour_plane_id);

    stream << P1(frame_num);

    if (!sps->frame_mbs_only_flag) {
        stream << P1(field_pic_flag);
        if (field_pic_flag)
            stream << P1(bottom_field_flag);
    }

    if (nal_unit_type == 5)
        stream << P1(idr_pic_id);

    if (sps->pic_order_cnt_type == 0) {
        stream << P1(pic_order_cnt_lsb);
        if (pps->bottom_field_pic_order_in_frame_present_flag && !field_pic_flag)
            stream << P1(delta_pic_order_cnt_bottom);
    }

    if (sps->pic_order_cnt_type == 1 && !sps->t1.delta_pic_order_always_zero_flag) {
        stream << P1(delta_pic_order_cnt[0]);
        if (pps->bottom_field_pic_order_in_frame_present_flag && !field_pic_flag)
            stream << P1(delta_pic_order_cnt[1]);
    }

    if (pps->redundant_pic_cnt_present_flag)
        stream << P1(redundant_pic_cnt);

    if (slice_type_e::B == slice_type)
        stream << P1(direct_spatial_mv_pred_flag);

    if ((slice_type_e::P  == slice_type) ||
        (slice_type_e::SP == slice_type) ||
        (slice_type_e::B  == slice_type)) {
        stream << P1(num_ref_idx_active_override_flag);
        if (num_ref_idx_active_override_flag) {
            stream << P1(num_ref_idx_l0_active_minus1);
            if (slice_type_e::B  == slice_type)
                stream << P1(num_ref_idx_l1_active_minus1);
        }
    }

    if ((slice_type_e::I != slice_type) && (slice_type_e::SI != slice_type))
        stream << rplm_l0.to_string();

    if (slice_type_e::B == slice_type)
        stream << rplm_l1.to_string();

    if ((pps->weighted_pred_flag && ((h264::slice_type_e::P == slice_type) || (h264::slice_type_e::SP == slice_type))) ||
        (pps->weighted_bipred_idc == 1 && (h264::slice_type_e::B == slice_type)))
        stream << pwt.to_string();

    if (nal_ref_idc != 0) {
        if (nal_unit_type == 5)
            stream << drpm_idr.to_string();
        else
            stream << drpm_nonidr.to_string();
    }

    if ((pps->entropy_coding_mode_flag) &&
        (slice_type_e::I != slice_type) && (slice_type_e::SI != slice_type))
        stream << P1(cabac_init_idc);

    stream << P1(slice_qp_delta);

    if ((slice_type_e::SI == slice_type) && (slice_type_e::SP == slice_type)) {
        if (slice_type_e::SP == slice_type)
            stream << P1(sp_for_switch_flag);
        stream << P1(slice_qs_delta);
    }

    if (pps->deblocking_filter_control_present_flag) {
        stream << P1(disable_deblocking_filter_idc);
        if (disable_deblocking_filter_idc != 1) {
            stream << P1(slice_alpha_c0_offset_div2);
            stream << P1(slice_beta_offset_div2);
        }
    }

    if ((pps->num_slice_groups_minus1 > 0) &&
        ((pps->slice_group_map_type >= 3) && (pps->slice_group_map_type <= 5)))
        stream << P1(slice_group_change_cycle);

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

#endif /* _SLICE_HEADER_HPP_ */
