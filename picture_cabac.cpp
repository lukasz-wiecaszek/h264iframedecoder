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
 * ctxIdxOffset: 70..72
 */
int picture_cabac::decode_mb_field_decoding_flag()
{
    int ctxIdxOffset = 70;
    int ctxIdxInc = 0;
    mb* curr_mb = m_context_variables.curr_mb;

    ctxIdxInc += (curr_mb->A != nullptr) && (curr_mb->A->type & MB_TYPE_INTERLACED);
    ctxIdxInc += (curr_mb->B != nullptr) && (curr_mb->B->type & MB_TYPE_INTERLACED);

    return m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
}

/**
 * 9.3.3.1.1.3 Derivation process of ctxIdxInc for the syntax element mb_type
 *
 * SI slices only
 *   Type of binarization: as specified in subclause 9.3.2.5
 *   maxBinIdxCtx:
 *     prefix: 0
 *     suffix: 6
 *   ctxIdxOffset:
 *     prefix: 0..2
 *     suffix: 3..10
 */
int picture_cabac::decode_mb_type_si_slice()
{
    int ctxIdxOffset = 0;
    int ctxIdxInc = 0;
    mb* curr_mb = m_context_variables.curr_mb;

    ctxIdxInc += ((curr_mb->left != nullptr) && ((curr_mb->left->type & MB_TYPE_SWITCHING) == 0));
    ctxIdxInc += ((curr_mb->top != nullptr) && ((curr_mb->top->type & MB_TYPE_SWITCHING) == 0));

    if (m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) == 0)
        return 0; /* SI 4x4 */

    return 1 + decode_mb_type_i_slice();
}

/**
 * 9.3.3.1.1.3 Derivation process of ctxIdxInc for the syntax element mb_type
 *
 * I slices only
 *   Type of binarization: as specified in subclause 9.3.2.5
 *   maxBinIdxCtx: 6
 *   ctxIdxOffset: 3..10
 */
int picture_cabac::decode_mb_type_i_slice()
{
    int ctxIdxOffset = 3;
    int ctxIdxInc = 0;
    mb* curr_mb = m_context_variables.curr_mb;

    ctxIdxInc += ((curr_mb->left != nullptr) && ((curr_mb->left->type & MB_TYPE_INTRA_NxN) == 0));
    ctxIdxInc += ((curr_mb->top != nullptr) && ((curr_mb->top->type & MB_TYPE_INTRA_NxN) == 0));

    if (m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) == 0)
        return 0; /* SI 4x4 */

    if (m_cabac_decoder.decode_terminate() == 1)
        return 25; /* PCM */

    int mb_type = 1; /* I16x16 */

    mb_type += 12 * m_cabac_decoder.decode_decision(ctxIdxOffset + 3); /* cbp_luma */
    if (m_cabac_decoder.decode_decision(ctxIdxOffset + 4)) /* cbp_chroma */
        mb_type += 4 + 4 * m_cabac_decoder.decode_decision(ctxIdxOffset + 5);

    mb_type += 2 * m_cabac_decoder.decode_decision(ctxIdxOffset + 6);
    mb_type += 1 * m_cabac_decoder.decode_decision(ctxIdxOffset + 7);

    return mb_type;

}

/**
 * 9.3.3.1.1.10 Derivation process of ctxIdxInc for the syntax element transform_size_8x8_flag
 *
 * Type of binarization: FL, cMax = 1
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 399
 */
int picture_cabac::decode_transform_size_8x8_flag()
{
    int ctxIdxOffset = 399;
    int ctxIdxInc = 0;
    mb* curr_mb = m_context_variables.curr_mb;

    ctxIdxInc += (curr_mb->A != nullptr) && (MB_IS_INTRA_8x8(curr_mb->A->type));
    ctxIdxInc += (curr_mb->B != nullptr) && (MB_IS_INTRA_8x8(curr_mb->B->type));

    return m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
}

/**
 * 9.3.3.1.1.4 Derivation process of ctxIdxInc for the syntax element coded_block_pattern
 *
 * Type of binarization: as specified in subclause 9.3.2.6
 * maxBinIdxCtx: 3
 * ctxIdxOffset: 73..76
 */
int picture_cabac::decode_cbp_luma()
{
    int cbp_a, cbp_b, cbp = 0;
    int ctxIdxOffset = 73;
    int ctxIdxInc = 0;
    mb* curr_mb = m_context_variables.curr_mb;

    if (curr_mb->left)
        cbp_a = (((curr_mb->left_pair[0]->cbp_luma >> (m_context_variables.left_blocks[0] & (~1))) & 2) << 0) |
                (((curr_mb->left_pair[1]->cbp_luma >> (m_context_variables.left_blocks[2] & (~1))) & 2) << 2);
    else
        cbp_a = 0x0F;

    if (curr_mb->top)
        cbp_b = curr_mb->top->cbp_luma;
    else
        cbp_b = 0x0F;

    ctxIdxInc = !(cbp_a & 0x02) + 2 * !(cbp_b & 0x04);
    cbp += m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
    ctxIdxInc = !(cbp   & 0x01) + 2 * !(cbp_b & 0x08);
    cbp += m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) << 1;
    ctxIdxInc = !(cbp_a & 0x08) + 2 * !(cbp   & 0x01);
    cbp += m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) << 2;
    ctxIdxInc = !(cbp   & 0x04) + 2 * !(cbp   & 0x02);
    cbp += m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) << 3;

    return cbp;
}

/**
 * 9.3.3.1.1.4 Derivation process of ctxIdxInc for the syntax element coded_block_pattern
 *
 * Type of binarization: as specified in subclause 9.3.2.6
 * maxBinIdxCtx: 1
 * ctxIdxOffset: 77..84
 */
int picture_cabac::decode_cbp_chroma()
{
    int cbp_a, cbp_b;
    int ctxIdxOffset = 77;
    int ctxIdxInc = 0;
    mb* curr_mb = m_context_variables.curr_mb;

    if (curr_mb->left)
        cbp_a = curr_mb->left->cbp_chroma & 0x03;
    else
        cbp_a = 0;

    if (curr_mb->top)
        cbp_b = curr_mb->top->cbp_chroma & 0x03;
    else
        cbp_b = 0;

    ctxIdxInc = 0;
    if (cbp_a > 0)
        ctxIdxInc++;
    if (cbp_b > 0)
        ctxIdxInc += 2;
    if (m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) == 0)
        return 0;

    ctxIdxInc = 4;
    if (cbp_a == 2)
        ctxIdxInc++;
    if (cbp_b == 2)
        ctxIdxInc += 2;
    return 1 + m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
}

/**
 * 9.3.3.1.1.5 Derivation process of ctxIdxInc for the syntax element mb_qp_delta
 *
 * Type of binarization: as specified in subclause 9.3.2.7
 * maxBinIdxCtx: 2
 * ctxIdxOffset: 60..63
 */
int picture_cabac::decode_mb_qp_delta()
{
    int qp_delta = 0;
    int ctxIdxOffset = 60;
    int ctxIdxInc;

    ctxIdxInc = (m_context_variables.lastQPdelta != 0) ? 1 : 0;
    if (m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc)) {
        ctxIdxInc = 2;
        qp_delta = 1;
        while (m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc)) {
            ctxIdxInc = 3;
            qp_delta++;
        }

        /* Table 9-3 – Assignment of syntax element to codeNum for signed Exp-Golomb coded syntax elements se(v) */
        if (qp_delta & 0x01)
            qp_delta =  ((qp_delta + 1) >> 1);
        else
            qp_delta = -((qp_delta + 1) >> 1);
    }

    return qp_delta;
}

/**
 * Type of binarization: FL, cMax = 1
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 68
 */
int picture_cabac::decode_prev_intra4x4_pred_mode_flag()
{
    int ctxIdxOffset = 68;
    int ctxIdxInc = 0;

    return m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
}

/**
 * Type of binarization: FL, cMax = 7
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 69
 */
int picture_cabac::decode_rem_intra4x4_pred_mode()
{
    int mode = 0;
    int ctxIdxOffset = 69;
    int ctxIdxInc = 0;

    mode += 1 * m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
    mode += 2 * m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
    mode += 4 * m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);

    return mode;
}

/**
 * Type of binarization: FL, cMax = 1
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 68
 */
int picture_cabac::decode_prev_intra8x8_pred_mode_flag()
{
    int ctxIdxOffset = 68;
    int ctxIdxInc = 0;

    return m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
}

/**
 * Type of binarization: FL, cMax = 7
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 69
 */
int picture_cabac::decode_rem_intra8x8_pred_mode()
{
    int mode = 0;
    int ctxIdxOffset = 69;
    int ctxIdxInc = 0;

    mode += 1 * m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
    mode += 2 * m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);
    mode += 4 * m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc);

    return mode;
}

/**
 * 9.3.3.1.1.8 Derivation process of ctxIdxInc for the syntax element intra_chroma_pred_mode
 *
 * Type of binarization: TU, cMax = 3
 * maxBinIdxCtx: 1
 * ctxIdxOffset: 64..67
 */
int picture_cabac::decode_intra_chroma_pred_mode()
{
    int ctxIdxOffset = 64;
    int ctxIdxInc = 0;
    mb* curr_mb = m_context_variables.curr_mb;

    /* No need to test for MB_IS_INTRA_NxN and MB_IS_INTRA_16x16,
       as intra_chroma_pred_mode shall be set to 0 for those macroblocks */

    ctxIdxInc += (curr_mb->left != nullptr) && (curr_mb->left->intra_chroma_pred_mode != 0);
    ctxIdxInc += (curr_mb->top != nullptr) && (curr_mb->top->intra_chroma_pred_mode != 0);

    if (m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) == 0)
        return 0;

    ctxIdxInc = 3;

    if (m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) == 0)
        return 1;

    if (m_cabac_decoder.decode_decision(ctxIdxOffset + ctxIdxInc) == 0)
        return 2;
    else
        return 3;
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

