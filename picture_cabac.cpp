/**
 * @file picture_cabac.cpp
 *
 * Definition of H.264 (ISO/IEC 14496-10) picture structure.
 *
 * Entropy decoding method to be applied for the syntax elements
 * for which two descriptors appear in the syntax tables is CABAC (see subclause 9.3).
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
#include "picture_cabac.hpp"
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
picture_cabac::picture_cabac(const h264_decoder& decoder, const h264::slice_header& sh) :
    picture{decoder, sh},
    m_cabac_decoder{}
{
}

picture_cabac::~picture_cabac()
{
}

void picture_cabac::decode(const h264::slice_header& sh, const h264::slice_data& sd)
{
    mb* mb;

    int slice_qp = std::clamp(
        m_context_variables.QPy - 6 * static_cast<int>(m_decoder.m_active_sps->bit_depth_luma_minus8), 0, 51);

    m_cabac_decoder.init_context_variables(sh, slice_qp);
    m_cabac_decoder.init_decoding_engine(sd);

    for (mb = curr_mb(); (mb = curr_mb()) != nullptr; advance_mb_pos()) {
        if ((sh.slice_type != h264::slice_type_e::I) && (sh.slice_type != h264::slice_type_e::SI)) {
            bool is_skipped = true;
            /* check for skipped macroblocks */
            if (is_skipped)
                continue;
        }

        std::cout << mb->to_string();
    }
}

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/
/**
 * 9.3.3.1.1.2 Derivation process of ctxIdxInc for the syntax element mb_field_decoding_flag
 *
 * Type of binarization: FL, cMax = 1
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 70
 */
int picture_cabac::decode_mb_field_decoding_flag()
{
    int ctxIdxInc = 0;
    mb* curr_mb = m_context_variables.curr_mb;

    ctxIdxInc += (curr_mb->A != nullptr) && (curr_mb->A->type & MB_TYPE_INTERLACED);
    ctxIdxInc += (curr_mb->B != nullptr) && (curr_mb->B->type & MB_TYPE_INTERLACED);

    return m_cabac_decoder.decode_decision(70 + ctxIdxInc);
}

int picture_cabac::decode_mb_type()
{
    return -1;
}

int picture_cabac::decode_transform_size_8x8_flag()
{
    return -1;
}

int picture_cabac::decode_cbp_luma()
{
    return -1;
}

int picture_cabac::decode_cbp_chroma()
{
    return -1;
}

int picture_cabac::decode_mb_qp_delta()
{
    return -1;
}

int picture_cabac::decode_prev_intra4x4_pred_mode_flag()
{
    return -1;
}

int picture_cabac::decode_rem_intra4x4_pred_mode()
{
    return -1;
}

int picture_cabac::decode_prev_intra8x8_pred_mode_flag()
{
    return -1;
}

int picture_cabac::decode_rem_intra8x8_pred_mode()
{
    return -1;
}


int picture_cabac::decode_intra_chroma_pred_mode()
{
    return -1;
}

int picture_cabac::decode_coded_block_flag()
{
    return -1;
}

int picture_cabac::decode_significant_coeff_flag()
{
    return -1;
}

int picture_cabac::decode_last_significant_coeff_flag()
{
    return -1;
}

int picture_cabac::decode_coeff_abs_level_minus1()
{
    return -1;
}

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/

