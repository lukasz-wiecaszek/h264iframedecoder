/**
 * @file h264_parser.cpp
 *
 * Brief description of what's inside this source file.
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

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_parser.hpp"
#include "nal_unit_type.hpp"
#include "ilog2.hpp"
#include "inverse_scanning_tables.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace ymn;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/
static int h264_parser_nal_to_rbsp(
    uint8_t* rbsp_buf, std::size_t rbsp_size, const uint8_t* nal_buf, std::size_t nal_size);

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
template<std::size_t N>
static inline void parse_scaling_list(istream_be& s, uint8_t (&coeffs)[N], const uint8_t (&dsl)[N])
{
    int32_t delta_scale;
    uint8_t last = 8, next = 8;

    const uint8_t* const scan = N == 16 ?
      h264::frame_scan_4x4 : h264::frame_scan_8x8;

    for (std::size_t i = 0; i < N; ++i) {
        if (next) {
            if (!s.read_exp_golomb_s(delta_scale)) {
                s.mark_corrupted();
                break;
            }
            next = (last + delta_scale) & 0xff;
        }
        if (!i && !next) {
            /* list is not transmited, we use the (d)efault (s)caling (l)ist (dsl) */
            memcpy(coeffs, dsl, N * sizeof(uint8_t));
            break;
        }
        last = coeffs[scan[i]] = next ? next : last;
    }
}

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
const h264_structure* h264_parser::get_structure(h264_parser_structure_e structure, int id)
{
    const h264_structure* retval = nullptr;

    switch (structure) {
        case h264_parser_structure_e::AUD:
            retval = &m_aud;
            break;

        case h264_parser_structure_e::SPS:
            if (id == H264_PARSER_STRUCTURE_ID_RECENT)
                id = m_recent_sps;
            if ((id >= 0) && (id < H264_PARSER_MAX_NUMBER_OF_SPS))
                retval = &m_sps_table[id];
            break;

        case h264_parser_structure_e::PPS:
            if (id == H264_PARSER_STRUCTURE_ID_RECENT)
                id = m_recent_pps;
            if ((id >= 0) && (id < H264_PARSER_MAX_NUMBER_OF_PPS))
                retval = &m_pps_table[id];
            break;

        case h264_parser_structure_e::SEI:
            retval = &m_sei;
            break;

        case h264_parser_structure_e::SLICE_HEADER:
            retval = &m_slice_header;
            break;

        case h264_parser_structure_e::SLICE_DATA:
            retval = &m_slice_data;
            break;

        default:
            return nullptr;
    }

    return retval && retval->is_valid() ? retval : nullptr;
}

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/
bool h264_parser::find_next_start_code()
{
    std::size_t i, limit, available;
    const uint8_t* p;

    available = m_flatbuffer.read_available();
    if (available < 3 /* start code prefix size */)
        return 0; /* not enough room to find start of a nal unit */

    limit = available - 3;

    p = m_flatbuffer.read_ptr();
    i = 0;
    while ((i <= limit) && (p[i] != 0x00 || p[i + 1] != 0x00 || p[i + 2] != 0x01))
        i++;

    if (i > limit)
        return false; /* have not found start of a nal unit */

    /* let's skip 3 bytes of start_code_prefix */
    i += 3;
    m_flatbuffer.consume(i);

    return true;
}

int h264_parser::find_nal_unit()
{
    std::size_t i, limit, available;
    const uint8_t* s;
    const uint8_t* p;

    s = m_flatbuffer.get_bookmark();
    if (NULL == s) {
        if (find_next_start_code()) {
            m_flatbuffer.set_bookmark();
            s = m_flatbuffer.get_bookmark();
        }
        else
            return 0; /* have not found start of a nal unit */
    }

    available = m_flatbuffer.read_available();
    if (available < 3)
        return 0; /* not enough room to find end of a nal unit */

    limit = available - 3;

    p = m_flatbuffer.read_ptr();
    i = 0;
    while ((i <= limit) && ((p[i] != 0x00) || (p[i + 1] != 0x00) || (p[i + 2] > 0x01)))
        i++;

    m_flatbuffer.consume(i);

    if (i > limit)
        return -1; /* have not found end of a nal unit */

    return &p[i] - s;
}

void h264_parser::parse_scaling_list_4x4(istream_be& s, h264::scaling_list_4x4 (&lists)[SL_4x4_NUM], int list)
{
    do {
        h264::scaling_list_4x4& sl = lists[list];

        if (!s.read_bits(1, sl.scaling_list_present_flag)) {
            s.mark_corrupted();
            break;
        }

        if (sl.scaling_list_present_flag)
            parse_scaling_list(s, sl.scaling_list, h264::scaling_list_default_4x4[list / 3]);

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        sl.set_valid(true);
    } while (0);
}

void h264_parser::parse_scaling_list_8x8(istream_be& s, h264::scaling_list_8x8 (&lists)[SL_8x8_NUM], int list)
{
    do {
        h264::scaling_list_8x8& sl = lists[list];

        if (!s.read_bits(1, sl.scaling_list_present_flag)) {
            s.mark_corrupted();
            break;
        }

        if (sl.scaling_list_present_flag)
            parse_scaling_list(s, sl.scaling_list, h264::scaling_list_default_8x8[list % 2]);

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        sl.set_valid(true);
    } while (0);
}

void h264_parser::parse_scaling_matrices_4x4(istream_be& s, h264::scaling_matrices& sm)
{
    do {
        parse_scaling_list_4x4(s, sm.scaling_matrices_4x4, SL_4x4_INTRA_Y);
        if (s.status() != ISTREAM_STATUS_OK)
            break;

        parse_scaling_list_4x4(s, sm.scaling_matrices_4x4, SL_4x4_INTRA_Cb);
        if (s.status() != ISTREAM_STATUS_OK)
            break;

        parse_scaling_list_4x4(s, sm.scaling_matrices_4x4, SL_4x4_INTRA_Cr);
        if (s.status() != ISTREAM_STATUS_OK)
            break;

        parse_scaling_list_4x4(s, sm.scaling_matrices_4x4, SL_4x4_INTER_Y);
        if (s.status() != ISTREAM_STATUS_OK)
            break;

        parse_scaling_list_4x4(s, sm.scaling_matrices_4x4, SL_4x4_INTER_Cb);
        if (s.status() != ISTREAM_STATUS_OK)
            break;

        parse_scaling_list_4x4(s, sm.scaling_matrices_4x4, SL_4x4_INTER_Cr);
        if (s.status() != ISTREAM_STATUS_OK)
            break;
    } while (0);
}

void h264_parser::parse_scaling_matrices_8x8(istream_be& s, h264::scaling_matrices& sm, uint32_t chroma_format_idc)
{
    do {
        parse_scaling_list_8x8(s, sm.scaling_matrices_8x8, SL_8x8_INTRA_Y);
        if (s.status() != ISTREAM_STATUS_OK)
            break;

        parse_scaling_list_8x8(s, sm.scaling_matrices_8x8, SL_8x8_INTER_Y);
        if (s.status() != ISTREAM_STATUS_OK)
            break;

        if (3 == chroma_format_idc) {
            parse_scaling_list_8x8(s, sm.scaling_matrices_8x8, SL_8x8_INTRA_Cb);
            if (s.status() != ISTREAM_STATUS_OK)
                break;

            parse_scaling_list_8x8(s, sm.scaling_matrices_8x8, SL_8x8_INTER_Cb);
            if (s.status() != ISTREAM_STATUS_OK)
                break;

            parse_scaling_list_8x8(s, sm.scaling_matrices_8x8, SL_8x8_INTRA_Cr);
            if (s.status() != ISTREAM_STATUS_OK)
                break;

            parse_scaling_list_8x8(s, sm.scaling_matrices_8x8, SL_8x8_INTER_Cr);
            if (s.status() != ISTREAM_STATUS_OK)
                break;
        }
    } while (0);
}

bool h264_parser::parse_scaling_matrices(istream_be& s, h264::scaling_matrices& sm, bool parse8x8, uint32_t chroma_format_idc)
{
    bool retval = false;

    do {
        parse_scaling_matrices_4x4(s, sm);
        if (s.status() != ISTREAM_STATUS_OK)
            break;

        if (parse8x8) {
            parse_scaling_matrices_8x8(s, sm, chroma_format_idc);
            if (s.status() != ISTREAM_STATUS_OK)
                break;
        }

        sm.set_valid(true);
        retval = true;
    } while (0);

    return retval;
}

bool h264_parser::parse_hrd_parameters(istream_be& s, h264::hrd_parameters& hrd)
{
    bool retval = false;

    do {
        if (!CHK_EXPR(s.read_exp_golomb_u(hrd.cpb_cnt_minus1), std::cout) ||
            !CHK_STRUCT_RANGE(hrd, cpb_cnt_minus1, 0U, 31U, std::cout)) {
            s.mark_corrupted();
            break;
        }

        s.read_bits(4, hrd.bit_rate_scale);
        s.read_bits(4, hrd.cpb_size_scale);

        for (uint32_t schedSelIdx = 0;
            (schedSelIdx <= hrd.cpb_cnt_minus1) && (s.status() == ISTREAM_STATUS_OK);
           ++schedSelIdx) {
            s.read_exp_golomb_u(hrd.bit_rate_value_minus1[schedSelIdx]);
            s.read_exp_golomb_u(hrd.cpb_size_value_minus1[schedSelIdx]);
            s.read_bits(1, hrd.cbr_flag[schedSelIdx]);
        }

        s.read_bits(5, hrd.initial_cpb_removal_delay_length_minus1);
        s.read_bits(5, hrd.cpb_removal_delay_length_minus1);
        s.read_bits(5, hrd.dpb_output_delay_length_minus1);
        s.read_bits(5, hrd.time_offset_length);

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        hrd.set_valid(true);
        retval = true;
    } while (0);

    return retval;
}

bool h264_parser::parse_vui_parameters(istream_be& s, h264::vui_parameters& vui)
{
    bool retval = false;

    do {
        s.read_bits(1, vui.aspect_ratio_info_present_flag);
        if (vui.aspect_ratio_info_present_flag) {
            s.read_bits(8, vui.aspect_ratio_idc);
            if (vui.aspect_ratio_idc == static_cast<uint32_t>(h264::aspect_ratio_idc_e::SAR_EXTENDED)) {
                s.read_bits(16, vui.sar_width);
                s.read_bits(16, vui.sar_height);
            }
        }

        s.read_bits(1, vui.overscan_info_present_flag);
        if (vui.overscan_info_present_flag)
            s.read_bits(1, vui.overscan_appropriate_flag);

        s.read_bits(1, vui.video_signal_type_present_flag);
        if (vui.video_signal_type_present_flag) {
            s.read_bits(3, vui.video_format);
            s.read_bits(1, vui.video_full_range_flag);
            s.read_bits(1, vui.colour_description_present_flag);
            if (vui.colour_description_present_flag) {
                s.read_bits(8, vui.colour_primaries);
                s.read_bits(8, vui.transfer_characteristics);
                s.read_bits(8, vui.matrix_coefficients);
            }
        }

        s.read_bits(1, vui.chroma_loc_info_present_flag);
        if (vui.chroma_loc_info_present_flag) {
            s.read_exp_golomb_u(vui.chroma_sample_loc_type_top_field);
            s.read_exp_golomb_u(vui.chroma_sample_loc_type_bottom_field);
        }

        s.read_bits(1, vui.timing_info_present_flag);
        if (vui.timing_info_present_flag) {
            s.read_bits(32, vui.num_units_in_tick);
            s.read_bits(32, vui.time_scale);
            s.read_bits(1, vui.fixed_frame_rate_flag);
        }

        s.read_bits(1, vui.nal_hrd_parameters_present_flag);
        if (vui.nal_hrd_parameters_present_flag)
            parse_hrd_parameters(s, vui.nal_hrd);

        s.read_bits(1, vui.vcl_hrd_parameters_present_flag);
        if (vui.vcl_hrd_parameters_present_flag)
            parse_hrd_parameters(s, vui.vcl_hrd);

        if (vui.nal_hrd_parameters_present_flag || vui.vcl_hrd_parameters_present_flag) {
            s.read_bits(1, vui.low_delay_hrd_flag);
        }

        s.read_bits(1, vui.pic_struct_present_flag);
        s.read_bits(1, vui.bitstream_restriction_flag);
        if (vui.bitstream_restriction_flag) {
            s.read_bits(1, vui.motion_vectors_over_pic_boundaries_flag);
            s.read_exp_golomb_u(vui.max_bytes_per_pic_denom);
            s.read_exp_golomb_u(vui.max_bits_per_mb_denom);
            s.read_exp_golomb_u(vui.log2_max_mv_length_horizontal);
            s.read_exp_golomb_u(vui.log2_max_mv_length_vertical);
            s.read_exp_golomb_u(vui.max_num_reorder_frames);
            s.read_exp_golomb_u(vui.max_dec_frame_buffering);
        }

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        vui.set_valid(true);
        retval = true;
    } while (0);

    return retval;
}

bool h264_parser::parse_ref_pic_list_modification(istream_be& s, h264::ref_pic_list_modification& rplm)
{
    bool retval = false;

    do {
        s.read_bits(1, rplm.ref_pic_list_modification_flag_lx);
        if (rplm.ref_pic_list_modification_flag_lx) {
            size_t i = 0;
            uint32_t modification_of_pic_nums_idc;
            uint32_t abs_diff_pic_num_minus1;
            uint32_t long_term_pic_num;

            do {
                s.read_exp_golomb_u(modification_of_pic_nums_idc);

                if (modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1)
                    s.read_exp_golomb_u(abs_diff_pic_num_minus1);
                else
                if (modification_of_pic_nums_idc == 2)
                    s.read_exp_golomb_u(long_term_pic_num);

                if (i < TABLE_ELEMENTS(rplm.m)) {
                    rplm.m[i].modification_of_pic_nums_idc = modification_of_pic_nums_idc;
                    if (modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1)
                        rplm.m[i].u.abs_diff_pic_num_minus1 = abs_diff_pic_num_minus1;
                    else
                    if (modification_of_pic_nums_idc == 2)
                        rplm.m[i].u.long_term_pic_num = long_term_pic_num;
                }

                i++;
            } while ((s.status() == ISTREAM_STATUS_OK) && (modification_of_pic_nums_idc != 3));
        }

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        rplm.set_valid(true);
        retval = true;
    }while (0);

    return retval;
}

static void h264_parser_parse_pred_weight_table_lx(
    istream_be& s, h264::pred_weight_table::lx& lx, uint32_t num_ref_idx_lx_active_minus1, bool chroma_components_present)
{
    for (uint32_t i = 0;
        (i <= num_ref_idx_lx_active_minus1) && (s.status() == ISTREAM_STATUS_OK);
       ++i) {
        s.read_bits(1, lx.luma_weight_lx_flag[i]);
        if (lx.luma_weight_lx_flag[i]) {
            s.read_exp_golomb_s(lx.luma_weight_lx[i]);
            s.read_exp_golomb_s(lx.luma_offset_lx[i]);
        }
        if (chroma_components_present) {
            s.read_bits(1, lx.chroma_weight_lx_flag[i]);
            if (lx.chroma_weight_lx_flag[i]) {
                s.read_exp_golomb_s(lx.chroma_weight_lx[i][0]);
                s.read_exp_golomb_s(lx.chroma_offset_lx[i][0]);
                s.read_exp_golomb_s(lx.chroma_weight_lx[i][1]);
                s.read_exp_golomb_s(lx.chroma_offset_lx[i][1]);
            }
        }
    }
}

bool h264_parser::parse_pred_weight_table(istream_be& s, h264::pred_weight_table& pwt)
{
    bool retval = false;

    do {
        s.read_exp_golomb_u(pwt.luma_log2_weight_denom);
        if (pwt.chroma_components_present)
            s.read_exp_golomb_u(pwt.chroma_log2_weight_denom);

        h264_parser_parse_pred_weight_table_lx(s, pwt.l0, pwt.num_ref_idx_l0_active_minus1, pwt.chroma_components_present);

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        if (h264::slice_type_e::B == pwt.slice_type)
            h264_parser_parse_pred_weight_table_lx(s, pwt.l1, pwt.num_ref_idx_l1_active_minus1, pwt.chroma_components_present);

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        pwt.set_valid(true);
        retval = true;
    } while (0);

    return retval;
}

bool h264_parser::parse_dec_ref_pic_marking_idr(istream_be& s, h264::dec_ref_pic_marking_idr& drpm)
{
    bool retval = false;

    do {
        s.read_bits(1, drpm.no_output_of_prior_pics_flag);
        s.read_bits(1, drpm.long_term_reference_flag);

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        drpm.set_valid(true);
        retval = true;
    } while (0);

    return retval;
}

bool h264_parser::parse_dec_ref_pic_marking_nonidr(istream_be& s, h264::dec_ref_pic_marking_nonidr& drpm)
{
    bool retval = false;

    do {
        s.read_bits(1, drpm.adaptive_ref_pic_marking_mode_flag);
        if (drpm.adaptive_ref_pic_marking_mode_flag) {
            size_t i = 0;
            uint32_t memory_management_control_operation;
            uint32_t difference_of_pic_nums_minus1;
            uint32_t long_term_pic_num;
            uint32_t long_term_frame_idx;
            uint32_t max_long_term_frame_idx_plus1;

            do {
                s.read_exp_golomb_u(memory_management_control_operation);

                difference_of_pic_nums_minus1 = 0;
                long_term_pic_num = 0;
                long_term_frame_idx = 0;
                max_long_term_frame_idx_plus1 = 0;

                if (memory_management_control_operation) {
                    if (memory_management_control_operation == 1 ||
                        memory_management_control_operation == 3)
                        s.read_exp_golomb_u(difference_of_pic_nums_minus1);

                    if (memory_management_control_operation == 2)
                        s.read_exp_golomb_u(long_term_pic_num);

                    if (memory_management_control_operation == 3 ||
                        memory_management_control_operation == 6)
                        s.read_exp_golomb_u(long_term_frame_idx);

                    if (memory_management_control_operation == 4)
                        s.read_exp_golomb_u(max_long_term_frame_idx_plus1);
                }

                if (i < TABLE_ELEMENTS(drpm.mmco)) {
                    drpm.mmco[i].memory_management_control_operation = memory_management_control_operation;
                    drpm.mmco[i].difference_of_pic_nums_minus1 = difference_of_pic_nums_minus1;
                    drpm.mmco[i].long_term_pic_num = long_term_pic_num;
                    drpm.mmco[i].long_term_frame_idx = long_term_frame_idx;
                    drpm.mmco[i].max_long_term_frame_idx_plus1 = max_long_term_frame_idx_plus1;
                }

                i++;
            } while ((s.status() == ISTREAM_STATUS_OK) && (memory_management_control_operation != 0));
        }

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        drpm.set_valid(true);
        retval = true;
    } while (0);

    return retval;
}

h264_parser_status_e h264_parser::parse_aud(istream_be& s)
{
    h264_parser_status_e retval = h264_parser_status_e::NAL_UNIT_CORRUPTED;

    do {
        h264::aud& aud = m_aud;
        aud.reset();

        if (!CHK_EXPR(s.read_bits(3, aud.primary_pic_type), std::cout) ||
            !CHK_STRUCT_RANGE(aud, primary_pic_type, 0U, 7U, std::cout)) {
            s.mark_corrupted();
            break;
        }

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        aud.set_valid(true);
        retval = h264_parser_status_e::AUD_PARSED;
    } while (0);

    return retval;
}

h264_parser_status_e h264_parser::parse_sps(istream_be& s)
{
    h264_parser_status_e retval = h264_parser_status_e::NAL_UNIT_CORRUPTED;

    do {
        bool status;
        uint8_t  profile_idc;
        uint8_t  constraint_flags;
        uint8_t  level_idc;
        uint32_t seq_parameter_set_id;

        s.read_u8(profile_idc);
        s.read_u8(constraint_flags);
        s.read_u8(level_idc);

        if (!CHK_EXPR(s.read_exp_golomb_u(seq_parameter_set_id), std::cout) ||
            !CHK_RANGE(seq_parameter_set_id, 0U, static_cast<unsigned>(H264_PARSER_MAX_NUMBER_OF_SPS - 1), std::cout)) {
            s.mark_corrupted();
            break;
        }

        h264::sps& sps = m_sps_table[seq_parameter_set_id];

        sps.profile_idc = profile_idc;
        sps.constraint_flags = constraint_flags;
        sps.level_idc = level_idc;
        sps.seq_parameter_set_id = seq_parameter_set_id;

        if (sps.profile_idc == 100 || sps.profile_idc == 110 ||
            sps.profile_idc == 122 || sps.profile_idc == 244 ||
            sps.profile_idc ==  44 || sps.profile_idc ==  83 ||
            sps.profile_idc ==  86 || sps.profile_idc == 118 ||
            sps.profile_idc == 128 || sps.profile_idc == 144) {

            s.read_exp_golomb_u(sps.chroma_format_idc);
            if (!CHK_STRUCT_RANGE(sps, chroma_format_idc, 0U, 3U, std::cout)) {
                s.mark_corrupted();
                break;
            }

            if (sps.chroma_format_idc == 3)
                s.read_bits(1, sps.separate_colour_plane_flag);
            else
                sps.separate_colour_plane_flag = 0;

            s.read_exp_golomb_u(sps.bit_depth_luma_minus8);
            if (!CHK_STRUCT_RANGE(sps, bit_depth_luma_minus8, 0U, 6U, std::cout)) {
                s.mark_corrupted();
                break;
            }

            s.read_exp_golomb_u(sps.bit_depth_chroma_minus8);
            if (!CHK_STRUCT_RANGE(sps, bit_depth_chroma_minus8, 0U, 6U, std::cout)) {
                s.mark_corrupted();
                break;
            }

            s.read_bits(1, sps.qpprime_y_zero_transform_bypass_flag);

            s.read_bits(1, sps.seq_scaling_matrix_present_flag);
            if (sps.seq_scaling_matrix_present_flag) {
                status = parse_scaling_matrices(s, sps.sm, true, sps.chroma_format_idc);
                if (!status)
                    break;
                /* for each list which is not transmited, use the fall-back scaling list
                   as defined in Table 7-2 – Assignment of mnemonic names to scaling list
                   indices and specification of fall-back rule */
                if (!sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Y].scaling_list_present_flag)
                    sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Y].copy(h264::scaling_list_default_4x4[0]);
                if (!sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cb].scaling_list_present_flag)
                    sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cb].copy(sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Y].scaling_list);
                if (!sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cr].scaling_list_present_flag)
                    sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cr].copy(sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cb].scaling_list);
                if (!sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Y].scaling_list_present_flag)
                    sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Y].copy(h264::scaling_list_default_4x4[1]);
                if (!sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cb].scaling_list_present_flag)
                    sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cb].copy(sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Y].scaling_list);
                if (!sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cr].scaling_list_present_flag)
                    sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cr].copy(sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cb].scaling_list);

                if (!sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Y].scaling_list_present_flag)
                    sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Y].copy(h264::scaling_list_default_8x8[0]);
                if (!sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Y].scaling_list_present_flag)
                    sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Y].copy(h264::scaling_list_default_8x8[1]);

                if (sps.chroma_format_idc == 3) {
                    if (!sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cb].scaling_list_present_flag)
                        sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cb].copy(sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Y].scaling_list);
                    if (!sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cb].scaling_list_present_flag)
                        sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cb].copy(sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Y].scaling_list);
                    if (!sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cr].scaling_list_present_flag)
                        sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cr].copy(sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cb].scaling_list);
                    if (!sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cr].scaling_list_present_flag)
                        sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cr].copy(sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cb].scaling_list);
                }
            }
            else
                sps.sm.set_defaults();
        }
        else {
            sps.chroma_format_idc = 1;
            sps.separate_colour_plane_flag = 0;
            sps.bit_depth_luma_minus8 = 0;
            sps.bit_depth_chroma_minus8 = 0;
            sps.qpprime_y_zero_transform_bypass_flag = 0;
            sps.seq_scaling_matrix_present_flag = 0;
            sps.sm.set_defaults();
        }

        s.read_exp_golomb_u(sps.log2_max_frame_num_minus4);
        if (!CHK_STRUCT_RANGE(sps, log2_max_frame_num_minus4, 0U, 12U, std::cout)) {
            s.mark_corrupted();
            break;
        }

        s.read_exp_golomb_u(sps.pic_order_cnt_type);
        if (!CHK_STRUCT_RANGE(sps, pic_order_cnt_type, 0U, 2U, std::cout)) {
            s.mark_corrupted();
            break;
        }

        if (0 == sps.pic_order_cnt_type)
            s.read_exp_golomb_u(sps.t0.log2_max_pic_order_cnt_lsb_minus4);
        else
        if (1 == sps.pic_order_cnt_type) {
            s.read_bits(1, sps.t1.delta_pic_order_always_zero_flag);
            s.read_exp_golomb_s(sps.t1.offset_for_non_ref_pic);
            s.read_exp_golomb_s(sps.t1.offset_for_top_to_bottom_field);
            s.read_exp_golomb_u(sps.t1.num_ref_frames_in_pic_order_cnt_cycle);
            if (!CHK_STRUCT_RANGE(sps, t1.num_ref_frames_in_pic_order_cnt_cycle, 0U, 255U, std::cout)) {
                s.mark_corrupted();
                break;
            }
            for (uint32_t i = 0; i < sps.t1.num_ref_frames_in_pic_order_cnt_cycle; ++i)
                if (!s.read_exp_golomb_s(sps.t1.offset_for_ref_frame[i]))
                    break;
        }
        else {
            /* do nothing */
        }

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        s.read_exp_golomb_u(sps.max_num_ref_frames);
        s.read_bits(1, sps.gaps_in_frame_num_value_allowed_flag);
        s.read_exp_golomb_u(sps.pic_width_in_mbs_minus1);
        s.read_exp_golomb_u(sps.pic_height_in_map_units_minus1);

        s.read_bits(1, sps.frame_mbs_only_flag);
        if (!sps.frame_mbs_only_flag)
            s.read_bits(1, sps.mb_adaptive_frame_field_flag);
        else
            sps.mb_adaptive_frame_field_flag = 0;

        s.read_bits(1, sps.direct_8x8_inference_flag);

        s.read_bits(1, sps.frame_cropping_flag);
        if (sps.frame_cropping_flag) {
            s.read_exp_golomb_u(sps.frame_crop_left_offset);
            s.read_exp_golomb_u(sps.frame_crop_right_offset);
            s.read_exp_golomb_u(sps.frame_crop_top_offset);
            s.read_exp_golomb_u(sps.frame_crop_bottom_offset);
        }
        else {
            sps.frame_crop_left_offset = 0;
            sps.frame_crop_right_offset = 0;
            sps.frame_crop_top_offset = 0;
            sps.frame_crop_bottom_offset = 0;
        }

        s.read_bits(1, sps.vui_parameters_present_flag);
        if (sps.vui_parameters_present_flag)
            parse_vui_parameters(s, sps.vui);

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        m_recent_sps = seq_parameter_set_id;

        sps.set_valid(true);
        retval = h264_parser_status_e::SPS_PARSED;
    } while (0);

    return retval;
}

h264_parser_status_e h264_parser::parse_pps(istream_be& s)
{
    h264_parser_status_e retval = h264_parser_status_e::NAL_UNIT_CORRUPTED;

    do {
        bool status;
        uint32_t pic_parameter_set_id;
        uint32_t seq_parameter_set_id;

        if (!CHK_EXPR(s.read_exp_golomb_u(pic_parameter_set_id), std::cout) ||
            !CHK_RANGE(pic_parameter_set_id, 0U, static_cast<unsigned>(H264_PARSER_MAX_NUMBER_OF_PPS - 1), std::cout)) {
            s.mark_corrupted();
            break;
        }

        h264::pps& pps = m_pps_table[pic_parameter_set_id];
        pps.reset();

        if (!CHK_EXPR(s.read_exp_golomb_u(seq_parameter_set_id), std::cout) ||
            !CHK_RANGE(seq_parameter_set_id, 0U, static_cast<unsigned>(H264_PARSER_MAX_NUMBER_OF_SPS - 1), std::cout)) {
            s.mark_corrupted();
            break;
        }

        h264::sps& sps = m_sps_table[seq_parameter_set_id];
        if (!sps.is_valid()) {
            std::cout << "error: pps #" << pic_parameter_set_id;
            std::cout << " refers to sps #" << seq_parameter_set_id << " which is not valid";
            std::cout << std::endl;
            break;
        }

        pps.pic_parameter_set_id = pic_parameter_set_id;
        pps.seq_parameter_set_id = seq_parameter_set_id;

        pps.set_active_sps(&m_sps_table[seq_parameter_set_id]);

        s.read_bits(1, pps.entropy_coding_mode_flag);
        s.read_bits(1, pps.bottom_field_pic_order_in_frame_present_flag);

        if (!CHK_EXPR(s.read_exp_golomb_u(pps.num_slice_groups_minus1), std::cout) ||
            !CHK_STRUCT_RANGE(pps, num_slice_groups_minus1, 0U, 7U, std::cout)) {
            s.mark_corrupted();
            break;
        }

        if (pps.num_slice_groups_minus1 > 0) {
            /* 7.4.2.2 Picture parameter set RBSP semantics */
            /* slice_group_map_type specifies how the mapping of slice group map */
            /* units to slice groups is coded. The value of slice_group_map_type */
            /* shall be in the range of 0 to 6, inclusive.*/
            if (!CHK_EXPR(s.read_exp_golomb_u(pps.slice_group_map_type), std::cout) ||
                !CHK_STRUCT_RANGE(pps, slice_group_map_type, 0U, 6U, std::cout)) {
                s.mark_corrupted();
                break;
            }

            if (pps.slice_group_map_type == 0) {
                for (uint32_t iGroup = 0; iGroup <= pps.num_slice_groups_minus1; ++iGroup)
                    s.read_exp_golomb_u(pps.t0.run_length_minus1[iGroup]);
            }
            else
            if (pps.slice_group_map_type == 2) {
                for (uint32_t iGroup = 0; iGroup < pps.num_slice_groups_minus1; ++iGroup) {
                    s.read_exp_golomb_u(pps.t1.top_left[iGroup]);
                    s.read_exp_golomb_u(pps.t1.bottom_right[iGroup]);
                }
            }
            else
            if ((pps.slice_group_map_type == 3) ||
                (pps.slice_group_map_type == 4) ||
                (pps.slice_group_map_type == 5)) {
                s.read_bits(1, pps.t2.slice_group_change_direction_flag);
                s.read_exp_golomb_u(pps.t2.slice_group_change_rate_minus1);
            }
            else
            if (pps.slice_group_map_type == 6) {
                s.read_exp_golomb_u(pps.t3.pic_size_in_map_units_minus1);
                for (uint32_t i = 0; i <= pps.t3.pic_size_in_map_units_minus1; ++i)
                    s.read_bits(ilog2_roundup(pps.num_slice_groups_minus1 + 1), pps.t3.slice_group_id[i]);
            }
        }

        if (!CHK_EXPR(s.read_exp_golomb_u(pps.num_ref_idx_l0_default_active_minus1), std::cout) ||
            !CHK_STRUCT_RANGE(pps, num_ref_idx_l0_default_active_minus1, 0U, 31U, std::cout)) {
            s.mark_corrupted();
            break;
        }

        if (!CHK_EXPR(s.read_exp_golomb_u(pps.num_ref_idx_l1_default_active_minus1), std::cout) ||
            !CHK_STRUCT_RANGE(pps, num_ref_idx_l1_default_active_minus1, 0U, 31U, std::cout)) {
            s.mark_corrupted();
            break;
        }

        s.read_bits(1, pps.weighted_pred_flag);
        s.read_bits(2, pps.weighted_bipred_idc);

        if (!CHK_EXPR(s.read_exp_golomb_s(pps.pic_init_qp_minus26), std::cout) ||
            !CHK_STRUCT_RANGE(pps, pic_init_qp_minus26, -26 - 6 * static_cast<int>(sps.bit_depth_luma_minus8), +25, std::cout)) {
            s.mark_corrupted();
            break;
        }

        if (!CHK_EXPR(s.read_exp_golomb_s(pps.pic_init_qs_minus26), std::cout) ||
            !CHK_STRUCT_RANGE(pps, pic_init_qs_minus26, -26, +25, std::cout)) {
            s.mark_corrupted();
            break;
        }

        if (!CHK_EXPR(s.read_exp_golomb_s(pps.chroma_qp_index_offset), std::cout) ||
            !CHK_STRUCT_RANGE(pps, chroma_qp_index_offset, -12, +12, std::cout)) {
            s.mark_corrupted();
            break;
        }

        s.read_bits(1, pps.deblocking_filter_control_present_flag);
        s.read_bits(1, pps.constrained_intra_pred_flag);
        s.read_bits(1, pps.redundant_pic_cnt_present_flag);

        if (more_rbsp_data(s)) {
            s.read_bits(1, pps.transform_8x8_mode_flag);

            s.read_bits(1, pps.pic_scaling_matrix_present_flag);
            if (pps.pic_scaling_matrix_present_flag) {
                status = parse_scaling_matrices(s, pps.sm, pps.transform_8x8_mode_flag, sps.chroma_format_idc);
                if (!status)
                    break;
                const uint8_t (&fallback_4x4_intra)[16] =
                    sps.seq_scaling_matrix_present_flag ?
                        sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Y].scaling_list : h264::scaling_list_default_4x4[0];
                const uint8_t (&fallback_4x4_inter)[16] =
                    sps.seq_scaling_matrix_present_flag ?
                        sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Y].scaling_list : h264::scaling_list_default_4x4[1];
                const uint8_t (&fallback_8x8_intra)[64] =
                    sps.seq_scaling_matrix_present_flag ?
                        sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Y].scaling_list : h264::scaling_list_default_8x8[0];
                const uint8_t (&fallback_8x8_inter)[64] =
                    sps.seq_scaling_matrix_present_flag ?
                        sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Y].scaling_list : h264::scaling_list_default_8x8[1];

                /* for each list which is not transmited, use the fall-back scaling list
                   as defined in Table 7-2 – Assignment of mnemonic names to scaling list
                   indices and specification of fall-back rule */
                if (!pps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Y].scaling_list_present_flag)
                    pps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Y].copy(fallback_4x4_intra);
                if (!pps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cb].scaling_list_present_flag)
                    pps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cb].copy(sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Y].scaling_list);
                if (!pps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cr].scaling_list_present_flag)
                    pps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cr].copy(sps.sm.scaling_matrices_4x4[SL_4x4_INTRA_Cb].scaling_list);
                if (!pps.sm.scaling_matrices_4x4[SL_4x4_INTER_Y].scaling_list_present_flag)
                    pps.sm.scaling_matrices_4x4[SL_4x4_INTER_Y].copy(fallback_4x4_inter);
                if (!pps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cb].scaling_list_present_flag)
                    pps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cb].copy(sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Y].scaling_list);
                if (!pps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cr].scaling_list_present_flag)
                    pps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cr].copy(sps.sm.scaling_matrices_4x4[SL_4x4_INTER_Cb].scaling_list);

                if (pps.transform_8x8_mode_flag) {
                    if (!pps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Y].scaling_list_present_flag)
                        pps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Y].copy(fallback_8x8_intra);
                    if (!pps.sm.scaling_matrices_8x8[SL_8x8_INTER_Y].scaling_list_present_flag)
                        pps.sm.scaling_matrices_8x8[SL_8x8_INTER_Y].copy(fallback_8x8_inter);

                    if (sps.chroma_format_idc == 3) {
                        if (!pps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cb].scaling_list_present_flag)
                            pps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cb].copy(sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Y].scaling_list);
                        if (!pps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cb].scaling_list_present_flag)
                            pps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cb].copy(sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Y].scaling_list);
                        if (!pps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cr].scaling_list_present_flag)
                            pps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cr].copy(sps.sm.scaling_matrices_8x8[SL_8x8_INTRA_Cb].scaling_list);
                        if (!pps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cr].scaling_list_present_flag)
                            pps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cr].copy(sps.sm.scaling_matrices_8x8[SL_8x8_INTER_Cb].scaling_list);
                    }
                 }
            }
            else
                pps.sm = sps.sm; /* copy scaling matrices from corresponding sps */

            s.read_exp_golomb_s(pps.second_chroma_qp_index_offset);
        }
        else {
            pps.transform_8x8_mode_flag = 0;
            pps.sm = sps.sm; /* copy scaling matrices from corresponding sps */
            pps.second_chroma_qp_index_offset = pps.chroma_qp_index_offset;
        }

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        m_recent_pps = pic_parameter_set_id;

        pps.set_valid(true);
        retval = h264_parser_status_e::PPS_PARSED;
    } while (0);

    return retval;
}

h264_parser_status_e h264_parser::parse_sei(istream_be& s)
{
    h264_parser_status_e retval = h264_parser_status_e::NAL_UNIT_CORRUPTED;

    do {
        bool status;
        uint8_t last;

        h264::sei& sei = m_sei;
        sei.reset();

        sei.payload_type = 0;
        sei.payload_size = 0;

        while ((true == (status = s.read_u8(last))) && 0xff == last)
            sei.payload_type += 255;

        if (status)
            sei.payload_type += last;
        else
            break;

        while ((true == (status = s.read_u8(last))) && 0xff == last)
            sei.payload_size += 255;

        if (status)
            sei.payload_size += last;
        else
            break;

        uint32_t i = 0;
        while ((i < sei.payload_size) && (i < sizeof(sei.payload_data)) && (true == (status = s.read_u8(sei.payload_data[i]))))
            ++i;

        if (!status)
            break;

        sei.set_valid(true);
        retval = h264_parser_status_e::SEI_PARSED;
    } while (0);

    return retval;
}

h264_parser_status_e h264_parser::parse_slice_header(istream_be& s, uint32_t nal_ref_idc, uint32_t nal_unit_type)
{
    h264_parser_status_e retval = h264_parser_status_e::NAL_UNIT_CORRUPTED;

    do {
        uint32_t pic_parameter_set_id;
        uint32_t seq_parameter_set_id;
        uint32_t slice_type;
        bool status;

        h264::slice_header& slice_header = m_slice_header;
        slice_header.reset();

        slice_header.nal_ref_idc = nal_ref_idc;
        slice_header.nal_unit_type = nal_unit_type;

        s.read_exp_golomb_u(slice_header.first_mb_in_slice);

        if (!CHK_EXPR(s.read_exp_golomb_u(slice_type), std::cout) ||
            !CHK_RANGE(slice_type, 0U, 9U, std::cout)) {
            s.mark_corrupted();
            break;
        }

        slice_header.slice_type = slice_type;

        if (!CHK_EXPR(s.read_exp_golomb_u(pic_parameter_set_id), std::cout) ||
            !CHK_RANGE(pic_parameter_set_id, 0U, static_cast<unsigned>(H264_PARSER_MAX_NUMBER_OF_PPS - 1), std::cout)) {
            s.mark_corrupted();
            break;
        }

        h264::pps& pps = m_pps_table[pic_parameter_set_id];
        if (!pps.is_valid()) {
            std::cout << "error: slice header refers to pps #" << pic_parameter_set_id << " which is not valid";
            std::cout << std::endl;
            break;
        }

        seq_parameter_set_id = pps.seq_parameter_set_id;

        h264::sps& sps = m_sps_table[seq_parameter_set_id];
        if (!sps.is_valid()) {
            std::cout << "error: pps #" << pic_parameter_set_id;
            std::cout << " refers to sps #" << seq_parameter_set_id << " which is not valid";
            std::cout << std::endl;
            break;
        }

        slice_header.pic_parameter_set_id = pic_parameter_set_id;
        slice_header.set_active_pps(&m_pps_table[pic_parameter_set_id]);

        if (sps.separate_colour_plane_flag)
            s.read_bits(2, slice_header.colour_plane_id);
        else
            slice_header.colour_plane_id = 3;//TODO: #define INVALID_COLOUR_PLANE_ID

        s.read_bits(sps.log2_max_frame_num_minus4 + 4, slice_header.frame_num);

        if (!sps.frame_mbs_only_flag) {
            s.read_bits(1, slice_header.field_pic_flag);
            if (slice_header.field_pic_flag)
                s.read_bits(1, slice_header.bottom_field_flag);
        }
        else
            slice_header.field_pic_flag = 0;

        if (nal_unit_type == 5)
            s.read_exp_golomb_u(slice_header.idr_pic_id);

        if (sps.pic_order_cnt_type == 0) {
            s.read_bits(sps.t0.log2_max_pic_order_cnt_lsb_minus4 + 4, slice_header.pic_order_cnt_lsb);
            if (pps.bottom_field_pic_order_in_frame_present_flag && !slice_header.field_pic_flag)
                s.read_exp_golomb_s(slice_header.delta_pic_order_cnt_bottom);
        }

        if (sps.pic_order_cnt_type == 1 && !sps.t1.delta_pic_order_always_zero_flag) {
            s.read_exp_golomb_s(slice_header.delta_pic_order_cnt[0]);
            if (pps.bottom_field_pic_order_in_frame_present_flag && !slice_header.field_pic_flag)
                s.read_exp_golomb_s(slice_header.delta_pic_order_cnt[1]);
        }

        if (pps.redundant_pic_cnt_present_flag)
            s.read_exp_golomb_u(slice_header.redundant_pic_cnt);

        if (h264::slice_type_e::B == slice_type)
            s.read_bits(1, slice_header.direct_spatial_mv_pred_flag);

        if (h264::slice_type_e::P  == slice_type ||
            h264::slice_type_e::SP == slice_type ||
            h264::slice_type_e::B  == slice_type) {
            s.read_bits(1, slice_header.num_ref_idx_active_override_flag);
            if (slice_header.num_ref_idx_active_override_flag) {
                if (!CHK_EXPR(s.read_exp_golomb_u(slice_header.num_ref_idx_l0_active_minus1), std::cout) ||
                    !CHK_STRUCT_RANGE(slice_header, num_ref_idx_l0_active_minus1, 0U, 31U, std::cout)) {
                    s.mark_corrupted();
                    break;
                }
                if (h264::slice_type_e::B == slice_type)
                    if (!CHK_EXPR(s.read_exp_golomb_u(slice_header.num_ref_idx_l1_active_minus1), std::cout) ||
                        !CHK_STRUCT_RANGE(slice_header, num_ref_idx_l1_active_minus1, 0U, 31U, std::cout)) {
                        s.mark_corrupted();
                        break;
                    }
            }
            else {
                slice_header.num_ref_idx_l0_active_minus1 = pps.num_ref_idx_l0_default_active_minus1;
                if (h264::slice_type_e::B == slice_type)
                    slice_header.num_ref_idx_l1_active_minus1 = pps.num_ref_idx_l1_default_active_minus1;
            }
        }

        if ((h264::slice_type_e::I != slice_type) &&
            (h264::slice_type_e::SI != slice_type)) {
            status = parse_ref_pic_list_modification(s, slice_header.rplm_l0);
            if (!status)
                break;
        }

        if (h264::slice_type_e::B == slice_type) {
            status = parse_ref_pic_list_modification(s, slice_header.rplm_l1);
            if (!status)
                break;
        }

        if ((pps.weighted_pred_flag && ((h264::slice_type_e::P == slice_type) || (h264::slice_type_e::SP == slice_type))) ||
            (pps.weighted_bipred_idc == 1 && (h264::slice_type_e::B == slice_type))) {
            h264::pred_weight_table& pwt = slice_header.pwt;

            pwt.slice_type = h264::to_slice_type_e(slice_type);
            pwt.chroma_components_present = sps.chroma_format_idc && !sps.separate_colour_plane_flag;
            pwt.num_ref_idx_l0_active_minus1 = slice_header.num_ref_idx_l0_active_minus1;
            pwt.num_ref_idx_l1_active_minus1 = slice_header.num_ref_idx_l1_active_minus1;

            status = parse_pred_weight_table(s, pwt);
            if (!status)
                break;
        }

        if (nal_ref_idc != 0) {
            if (nal_unit_type == 5)
                parse_dec_ref_pic_marking_idr(s, slice_header.drpm_idr);
            else
                parse_dec_ref_pic_marking_nonidr(s, slice_header.drpm_nonidr);
        }

        if ((pps.entropy_coding_mode_flag) &&
            (h264::slice_type_e::I != slice_type) && (h264::slice_type_e::SI != slice_type)) {
            if (!CHK_EXPR(s.read_exp_golomb_u(slice_header.cabac_init_idc), std::cout) ||
                !CHK_STRUCT_RANGE(slice_header, cabac_init_idc, 0U, 2U, std::cout)) {
                s.mark_corrupted();
                break;
            }
        }

        if (!CHK_EXPR(s.read_exp_golomb_s(slice_header.slice_qp_delta), std::cout) ||
            !CHK_STRUCT_RANGE(slice_header, slice_qp_delta,
                -26 - 6 * static_cast<int>(sps.bit_depth_luma_minus8) - pps.pic_init_qp_minus26,
                 25 - pps.pic_init_qp_minus26, std::cout)) {
            s.mark_corrupted();
            break;
        }

        if ((h264::slice_type_e::SI == slice_type) && (h264::slice_type_e::SP == slice_type)) {
            if (h264::slice_type_e::SP == slice_type)
                s.read_bits(1, slice_header.sp_for_switch_flag);
            s.read_exp_golomb_s(slice_header.slice_qs_delta);
        }

        if (pps.deblocking_filter_control_present_flag) {
            s.read_exp_golomb_u(slice_header.disable_deblocking_filter_idc);
            if (slice_header.disable_deblocking_filter_idc != 1) {
                s.read_exp_golomb_s(slice_header.slice_alpha_c0_offset_div2);
                s.read_exp_golomb_s(slice_header.slice_beta_offset_div2);
            }
        }

        if ((pps.num_slice_groups_minus1 > 0) &&
            ((pps.slice_group_map_type >= 3) && (pps.slice_group_map_type <= 5))) {
            s.read_bits(
                ilog2(pps.t2.slice_group_change_rate_minus1 + 1),
                slice_header.slice_group_change_cycle);
        }

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        set_slice_data(s);

        slice_header.set_valid(true);
        retval = h264_parser_status_e::SLICE_PARSED;
    } while (0);

    return retval;
}

h264_parser_status_e h264_parser::parse_nal_unit(istream_be& s)
{
    uint8_t nal_header;
    uint32_t nal_ref_idc;
    uint32_t nal_unit_type;
    bool status;

    status = s.read_u8(nal_header);
    if (!status)
        return h264_parser_status_e::NAL_UNIT_CORRUPTED;

    nal_ref_idc = (nal_header & 0x60) >> 5;
    nal_unit_type = nal_header & 0x1f;

    std::cout << std::endl;
    std::cout << "nal_unit_type: " << nal_unit_type;
    std::cout << " '" << ymn::to_string(static_cast<nal_unit_type_e>(nal_unit_type)) << "'";
    std::cout << " size: " << s.size();
    std::cout << std::endl;

    switch (static_cast<nal_unit_type_e>(nal_unit_type)) {
        case nal_unit_type_e::AUD:
            return parse_aud(s);

        case nal_unit_type_e::SPS:
            return parse_sps(s);

        case nal_unit_type_e::PPS:
            return parse_pps(s);

        case nal_unit_type_e::SEI:
            return parse_sei(s);

        case nal_unit_type_e::CODED_SLICE_NON_IDR_PICTURE:
        case nal_unit_type_e::CODED_SLICE_IDR_PICTURE:
            return parse_slice_header(s, nal_ref_idc, nal_unit_type);

        default:
            break;
    }

    return h264_parser_status_e::NAL_UNIT_SKIPPED;
}

h264_parser_status_e h264_parser::parse_nal_units()
{
    h264_parser_status_e status = h264_parser_status_e::NEED_BYTES;

    return status;
}

h264_parser_status_e h264_parser::parse_byte_stream_nal_units()
{
    h264_parser_status_e status = h264_parser_status_e::NEED_BYTES;
    int num_bytes_in_nal_unit;

    if ((num_bytes_in_nal_unit = find_nal_unit()) > 0) {
        int rbsp_size;
        uint8_t* rbsp_buf;
        const uint8_t* p = m_flatbuffer.get_bookmark();

        rbsp_buf = new uint8_t[num_bytes_in_nal_unit];
        rbsp_size = h264_parser_nal_to_rbsp(rbsp_buf, num_bytes_in_nal_unit, p, num_bytes_in_nal_unit);

        if (rbsp_size > 0) {
            istream_be s(rbsp_buf, rbsp_size);

            status = parse_nal_unit(s);
        }
        else
            status = h264_parser_status_e::NAL_UNIT_CORRUPTED;

        delete [] rbsp_buf;
        m_flatbuffer.clear_bookmark();
    }

    return status;
}

void h264_parser::set_slice_data(istream_be& s)
{
    do {
        h264::slice_data& slice_data = m_slice_data;
        slice_data.reset();

        if (s.status() != ISTREAM_STATUS_OK)
            break;

        m_slice_data.data = s.current_data_pointer();
        m_slice_data.size = s.remains();
        m_slice_data.bit_pos = s.tell_bits();

        slice_data.set_valid(true);
    } while (0);
}

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/
/**
 * Converts NAL data to RBSP data.
 *
 * The size of 'rbsp_buf' cannot be smaller than the size of the 'nal_buf'
 * to guarantee the output will fit.
 * If that is not true, output may be truncated and an error will be returned.
 * Additionally, certain byte sequences in the input 'nal_buf'
 * are not allowed in the spec and also cause the conversion to fail
 * and an error to be returned.
 *
 * @param[in,out] rbsp_buf  Pointer to the buffer in which to put the rbsp data.
 * @param[in]     rbsp_size Maximum size of the rbsp data.
 * @param[in]     nal_buf   Pointer to the buffer containing nal data.
 * @param[in]     nal_size  Size of the nal data.
 *
 * @return Actual size of rbsp data, or -1 on error.
 *
 * @note 7.3.1 NAL unit syntax
 * @note 7.4.1.1 Encapsulation of an SODB within an RBSP
 */
static int h264_parser_nal_to_rbsp(
    uint8_t* rbsp_buf, std::size_t rbsp_size, const uint8_t* nal_buf, std::size_t nal_size)
{
    std::size_t i     = 0;
    std::size_t j     = 0;
    std::size_t count = 0;

    while (i < nal_size) {
        /* within the NAL unit, the following three-byte sequences shall not occur */
        /* at any byte-aligned position: 0x000000, 0x000001, 0x000002 */
        if ((count == 2) && (nal_buf[i] < 0x03))
            return -1;

        if ((count == 2) && (nal_buf[i] == 0x03)) {
            /* within the NAL unit, any four-byte sequence that starts with 0x000003 */
            /* other than the following sequences shall not occur at any byte-aligned position: */
            /* 0x00000300, 0x00000301, 0x00000302, 0x00000303 */
            /* check the 4th byte after 0x000003, except when cabac_zero_word is used, */
            /* in which case the last three bytes of this NAL unit must be 0x000003 */
            if((i < nal_size - 1) && (nal_buf[i + 1] > 0x03))
                return -1;

            /* if cabac_zero_word is used, the final byte of this NAL unit(0x03) is discarded, */
            /* and the last two bytes of RBSP must be 0x0000 */
            if (i == nal_size - 1)
                break;

            i++;
            count = 0;
        }

        if (j >= rbsp_size)
            return -1; /* error, not enough space */

        rbsp_buf[j] = nal_buf[i];

        if(nal_buf[i] == 0x00)
            count++;
        else
            count = 0;

        i++;
        j++;
    }

    return j;
}

