/**
 * @file picture.cpp
 *
 * Definition of H.264 (ISO/IEC 14496-10) picture structure.
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
#include "picture.hpp"
#include "h264_decoder.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace ymn::h264;

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

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
picture::picture(const h264_decoder& decoder, const h264::slice_header& sh) :
    m_decoder{decoder},
    m_picture_structure{},
    m_context_variables{},
    m_mbs{nullptr}
{
    init_coxtext_variables(sh);
    m_mbs = new h264::mb[m_decoder.m_dimensions.mb_num];
}

picture::~picture()
{
    delete [] m_mbs;
}

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/
void picture::init_coxtext_variables(const h264::slice_header& sh)
{
    const sps* sps = m_decoder.m_active_sps;
    const pps* pps = m_decoder.m_active_pps;

    if (sps->frame_mbs_only_flag) {
        m_picture_structure = picture_structure_e::frame;
        m_context_variables.mb_aff_frame = false;
    }
    else {
        if (sh.field_pic_flag) {
            if (sh.bottom_field_flag)
                m_picture_structure = picture_structure_e::field_bottom;
            else
                m_picture_structure = picture_structure_e::field_top;
            m_context_variables.mb_aff_frame = false;
        }
        else {
            m_picture_structure = picture_structure_e::frame;
            m_context_variables.mb_aff_frame = sps->mb_adaptive_frame_field_flag;
        }
    }

    m_context_variables.mb_field_decoding_flag = m_picture_structure != picture_structure_e::frame;

    m_context_variables.mb_x = sh.first_mb_in_slice % m_decoder.m_dimensions.mb_width;
    m_context_variables.mb_y = sh.first_mb_in_slice / m_decoder.m_dimensions.mb_width;
    m_context_variables.mb_pos =
        m_context_variables.mb_x +
        m_context_variables.mb_y * m_decoder.m_dimensions.mb_width;

    m_context_variables.curr_mb = nullptr; // it will be evaluated in next_mb()

    if (sh.field_pic_flag || m_context_variables.mb_aff_frame)
        m_context_variables.mb_y *= 2;

    if (m_picture_structure == picture_structure_e::field_bottom)
        m_context_variables.mb_y +=1;

    m_context_variables.curr_mb = nullptr;


    m_context_variables.lastQPdelta = 0;
    m_context_variables.QPy = pps->pic_init_qp_minus26 + 26 + sh.slice_qp_delta;
    m_context_variables.QPc[0] = m_decoder.m_chroma_qp_table[0][m_context_variables.QPy];
    m_context_variables.QPc[1] = m_decoder.m_chroma_qp_table[1][m_context_variables.QPy];

    m_context_variables.chroma_array_type = sps->chroma_format_idc;
    if (sps->chroma_format_idc == 3 && sps->separate_colour_plane_flag)
        m_context_variables.chroma_array_type = 0;

    m_context_variables.left_blocks = nullptr; // it will be evaluated in calculate_neighbours_part2()

    //m_context_variables.intraNxN_pred_mode_cache - it will be initialized during processing of macroblock
    //m_context_variables.non_zero_count           - it will be initialized during processing of macroblock
    //m_context_variables.coeffs_ac                - it will be initialized during processing of macroblock
    //m_context_variables.coeffs_dc                - it will be initialized during processing of macroblock
}

void picture::calculate_neighbours_part1()
{
    int y = m_context_variables.mb_y;
    int n = m_context_variables.mb_pos;
    int mb_width = m_decoder.m_dimensions.mb_width;
    mb& mb = m_mbs[n];

    if (m_context_variables.mb_aff_frame) {
        int shift = y & 1 ? 3 * mb_width : 2 * mb_width;

        mb.A = (n % mb_width) ? get_mb(n - 1 - (y & 1) * mb_width) : nullptr;
        mb.B = get_mb(n - shift);
        mb.C = ((n + 1) % mb_width) ? get_mb(n - shift + 1) : nullptr;
        mb.D = (n % mb_width) ? get_mb(n - shift - 1) : nullptr;
    }
    else {
        if (m_picture_structure == picture_structure_e::frame) {
            mb.A = (n % mb_width) ? get_mb(n - 1) : nullptr;
            mb.B = get_mb(n - mb_width);
            mb.C = ((n + 1) % mb_width) ? get_mb(n - mb_width + 1) : nullptr;
            mb.D = (n % mb_width) ? get_mb(n - mb_width - 1) : nullptr;
        }
        else {
            int shift = 2 * mb_width;

            mb.A = (n % mb_width) ? get_mb(n - 1) : nullptr;
            mb.B = get_mb(n - shift);
            mb.C = ((n + 1) % mb_width) ? get_mb(n - shift + 1) : nullptr;
            mb.D = (n % mb_width) ? get_mb(n - shift - 1) : nullptr;
        }
    }

    /* To calculate left and top references it is necessary to know
    whether the current macroblock is a frame or field macroblock.
    This is why left and top references will be calculated later (in part2)
    when current macroblock frame/field status is already known. */
    mb.left = nullptr;
    mb.left_pair[0] = nullptr;
    mb.left_pair[1] = nullptr;
    mb.top = nullptr;
}

void picture::calculate_neighbours_part2()
{
    mb *curr_mb = m_context_variables.curr_mb;
    int mb_width = m_decoder.m_dimensions.mb_width;

    static const uint8_t left_block_options[4][32] =
    {
        {0, 1, 2, 3, 7, 10, 8, 11, 3 + 0 * 4, 3 + 1 * 4, 3 + 2 * 4, 3 + 3 * 4, 1 + 4 * 4, 1 + 8 * 4, 1 + 5 * 4, 1 + 9 * 4},
        {2, 2, 3, 3, 8, 11, 8, 11, 3 + 2 * 4, 3 + 2 * 4, 3 + 3 * 4, 3 + 3 * 4, 1 + 5 * 4, 1 + 9 * 4, 1 + 5 * 4, 1 + 9 * 4},
        {0, 0, 1, 1, 7, 10, 7, 10, 3 + 0 * 4, 3 + 0 * 4, 3 + 1 * 4, 3 + 1 * 4, 1 + 4 * 4, 1 + 8 * 4, 1 + 4 * 4, 1 + 8 * 4},
        {0, 2, 0, 2, 7, 10, 7, 10, 3 + 0 * 4, 3 + 2 * 4, 3 + 0 * 4, 3 + 2 * 4, 1 + 4 * 4, 1 + 8 * 4, 1 + 4 * 4, 1 + 8 * 4}
    };

    if (m_context_variables.mb_aff_frame) {
        mb *mb_ax;
        mb *mb_bx;
        if (m_context_variables.mb_field_decoding_flag == 0) { /* currMbFrameFlag '1' */
            if ((curr_mb->y & 1) == 0) { /* mbIsTopMbFlag '1' */
                mb_ax = curr_mb->A;
                mb_bx = curr_mb->B;
                if (mb_ax) {
                    if ((mb_ax->type & MB_TYPE_INTERLACED) == 0) {
                        curr_mb->left_pair[0] = mb_ax;
                        curr_mb->left_pair[1] = mb_ax;
                        m_context_variables.left_blocks = left_block_options[0];
                    }
                    else {
                        curr_mb->left_pair[0] = mb_ax;
                        curr_mb->left_pair[1] = mb_ax;
                        m_context_variables.left_blocks = left_block_options[2];
                    }
                }
                if (mb_bx)
                    curr_mb->top = mb_bx+mb_width;
            }
            else { /* mbIsTopMbFlag '0' */
                mb_ax = curr_mb->A;
                mb_bx = curr_mb;
                if (mb_ax) {
                    if ((mb_ax->type & MB_TYPE_INTERLACED) == 0) {
                        curr_mb->left_pair[0] = mb_ax+mb_width;
                        curr_mb->left_pair[1] = mb_ax+mb_width;
                        m_context_variables.left_blocks = left_block_options[0];
                    }
                    else {
                        curr_mb->left_pair[0] = mb_ax;
                        curr_mb->left_pair[1] = mb_ax;
                        m_context_variables.left_blocks = left_block_options[1];
                    }
                }
                if (mb_bx)
                    curr_mb->top = mb_bx-mb_width;
            }
        }
        else { /* currMbFrameFlag '0' */
            if ((curr_mb->y & 1) == 0) { /* mbIsTopMbFlag '1' */
                mb_ax = curr_mb->A;
                mb_bx = curr_mb->B;
                if (mb_ax) {
                    if ((mb_ax->type & MB_TYPE_INTERLACED) == 0) {
                        curr_mb->left_pair[0] = mb_ax;
                        curr_mb->left_pair[1] = mb_ax+mb_width;
                        m_context_variables.left_blocks = left_block_options[3];
                    }
                    else {
                        curr_mb->left_pair[0] = mb_ax;
                        curr_mb->left_pair[1] = mb_ax;
                        m_context_variables.left_blocks = left_block_options[0];
                    }
                }
                if (mb_bx) {
                    if ((mb_bx->type & MB_TYPE_INTERLACED) == 0)
                        curr_mb->top = mb_bx+mb_width;
                    else
                        curr_mb->top = mb_bx;
                }
            }
            else { /* mbIsTopMbFlag '0' */
                mb_ax = curr_mb->A;
                mb_bx = curr_mb->B;
                if (mb_ax) {
                    if ((mb_ax->type & MB_TYPE_INTERLACED) == 0) {
                        curr_mb->left_pair[0] = mb_ax;
                        curr_mb->left_pair[1] = mb_ax+mb_width;
                        m_context_variables.left_blocks = left_block_options[3];
                    }
                    else {
                        curr_mb->left_pair[0] = mb_ax+mb_width;
                        curr_mb->left_pair[1] = mb_ax+mb_width;
                        m_context_variables.left_blocks = left_block_options[0];
                    }
                }
                if (mb_bx)
                    curr_mb->top = mb_bx+mb_width;
            }
        }
        curr_mb->left = curr_mb->left_pair[0];
    }
    else {
        curr_mb->left = curr_mb->A;
        curr_mb->left_pair[0] = curr_mb->A;
        curr_mb->left_pair[1] = curr_mb->A;
        curr_mb->top = curr_mb->B;
        m_context_variables.left_blocks = left_block_options[0];
    }
}

void picture::advance_mb_pos()
{
    if (m_context_variables.mb_aff_frame) {
        if ((m_context_variables.mb_y&1) == 0)
            m_context_variables.mb_y++; /* move to bottom macroblock of a macroblock pair */
        else {
            m_context_variables.mb_y--; /* move to top macroblock of a macroblock pair */
            m_context_variables.mb_x++; /* move to right macroblock pair */
            if (m_context_variables.mb_x >= m_decoder.m_dimensions.mb_width) {
                m_context_variables.mb_x = 0;
                m_context_variables.mb_y +=2;
            }
        }
    }
    else {
        m_context_variables.mb_x++; /* move to right macroblock */
        if (m_context_variables.mb_x >= m_decoder.m_dimensions.mb_width) {
            m_context_variables.mb_x = 0;
            m_context_variables.mb_y++;
            if (m_picture_structure != picture_structure_e::frame)
                m_context_variables.mb_y++;
        }
    }

    m_context_variables.mb_pos =
        m_context_variables.mb_x +
        m_context_variables.mb_y * m_decoder.m_dimensions.mb_width;
}

mb* picture::curr_mb()
{
    mb *curr_mb;
    int n = m_context_variables.mb_pos;

    if (n < m_decoder.m_dimensions.mb_num)
        curr_mb = &m_mbs[n];
    else
        curr_mb = nullptr;

    m_context_variables.curr_mb = curr_mb;

    if (curr_mb != nullptr) {
        curr_mb->x = m_context_variables.mb_x;
        curr_mb->y = m_context_variables.mb_y;
        curr_mb->pos = m_context_variables.mb_pos;
        curr_mb->slice_num = 0;

        calculate_neighbours_part1();
    }

    return curr_mb;
}

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/

