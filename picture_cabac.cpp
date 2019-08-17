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
#include "colour_component.hpp"
#include "h264_decoder.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace ymn::h264;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define CAT_16x16_DC_Y    0
#define CAT_16x16_AC_Y    1
#define CAT_4x4_Y         2
#define CAT_CHROMA_DC     3
#define CAT_CHROMA_AC     4
#define CAT_8x8_Y         5
#define CAT_16x16_DC_Cb   6
#define CAT_16x16_AC_Cb   7
#define CAT_4x4_Cb        8
#define CAT_8x8_Cb        9
#define CAT_16x16_DC_Cr  10
#define CAT_16x16_AC_Cr  11
#define CAT_4x4_Cr       12
#define CAT_8x8_Cr       13

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
void picture_cabac::non_zero_count_cache_init(uint32_t mb_type)
{
    const mb* curr_mb = m_context_variables.curr_mb;
    const uint8_t* left_blocks = m_context_variables.left_blocks;

    mb_cache& nzc_cache_y  = m_context_variables.non_zero_count[to_int(colour_component_e::Y)];
    mb_cache& nzc_cache_cb = m_context_variables.non_zero_count[to_int(colour_component_e::Cb)];
    mb_cache& nzc_cache_cr = m_context_variables.non_zero_count[to_int(colour_component_e::Cr)];

    if (curr_mb->top) {
        const uint8_t* nzc = curr_mb->top->non_zero_count;

        std::memcpy(&nzc_cache_y[0 * 8 + 4], &nzc[3 * 4], 4);

        if (m_context_variables.chroma_array_type == 1 ||
            m_context_variables.chroma_array_type == 2) {
            std::memcpy(&nzc_cache_cb[0 * 8 + 4], &nzc[5 * 4], 4);
            std::memcpy(&nzc_cache_cr[0 * 8 + 4], &nzc[9 * 4], 4);
        }
        else
        if (m_context_variables.chroma_array_type == 3) {
            std::memcpy(&nzc_cache_cb[0 * 8 + 4], &nzc[ 7 * 4], 4);
            std::memcpy(&nzc_cache_cr[0 * 8 + 4], &nzc[11 * 4], 4);
        }
        else {
            /* do nothing */
        }
    }
    else {
        // 0x40 (64) means "value not available"
        uint32_t top_mb_not_available = m_decoder.m_active_pps->entropy_coding_mode_flag && !MB_IS_INTRA(mb_type) ? 0 : 0x40404040;

#define DEREFERENCE(TYPE, PTR) (*(reinterpret_cast<TYPE*>(PTR)))

        DEREFERENCE(uint32_t, &nzc_cache_y [0 * 8 + 4]) = top_mb_not_available;
        DEREFERENCE(uint32_t, &nzc_cache_cb[0 * 8 + 4]) = top_mb_not_available;
        DEREFERENCE(uint32_t, &nzc_cache_cr[0 * 8 + 4]) = top_mb_not_available;

#undef DEREFERENCE
    }

    for (int i = 0; i < 2; ++i) {
        if (curr_mb->left_pair[i]) {
            const uint8_t* nzc = curr_mb->left_pair[i]->non_zero_count;

            nzc_cache_y[1 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 0 + 2 * i]];
            nzc_cache_y[2 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 1 + 2 * i]];

            if (m_context_variables.chroma_array_type == 1) {
                nzc_cache_cb[1 * 8 + 3 + 8 * i] = nzc[left_blocks[8 + 4 + 2 * i]];
                nzc_cache_cr[1 * 8 + 3 + 8 * i] = nzc[left_blocks[8 + 5 + 2 * i]];
            }
            else
            if (m_context_variables.chroma_array_type == 2) {
                nzc_cache_cb[1 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 0 + 2 * i] - 2 + 4 * 4];
                nzc_cache_cb[2 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 1 + 2 * i] - 2 + 4 * 4];
                nzc_cache_cr[1 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 0 + 2 * i] - 2 + 8 * 4];
                nzc_cache_cr[2 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 1 + 2 * i] - 2 + 8 * 4];
            }
            else
            if (m_context_variables.chroma_array_type == 3) {
                nzc_cache_cb[1 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 0 + 2 * i] + 4 * 4];
                nzc_cache_cb[2 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 1 + 2 * i] + 4 * 4];
                nzc_cache_cr[1 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 0 + 2 * i] + 8 * 4];
                nzc_cache_cr[2 * 8 + 3 + 2 * 8 * i] = nzc[left_blocks[8 + 1 + 2 * i] + 8 * 4];
            }
            else {
                /* do nothing */
            }
        }
        else {
            nzc_cache_y [1 * 8 + 3 + 2 * 8 * i] =
            nzc_cache_y [2 * 8 + 3 + 2 * 8 * i] =
            nzc_cache_cb[1 * 8 + 3 + 2 * 8 * i] =
            nzc_cache_cb[2 * 8 + 3 + 2 * 8 * i] =
            nzc_cache_cr[1 * 8 + 3 + 2 * 8 * i] =
            nzc_cache_cr[2 * 8 + 3 + 2 * 8 * i] =
                m_decoder.m_active_pps->entropy_coding_mode_flag && !MB_IS_INTRA(mb_type) ? 0 : 0x40;
        }
    }

    if ((m_context_variables.chroma_array_type == 3) && (MB_IS_INTRA_8x8(mb_type))) {
        //TODO: implement this part
    }
}

void picture_cabac::non_zero_count_save()
{
    mb* curr_mb = m_context_variables.curr_mb;
    uint8_t* nzc = curr_mb->non_zero_count;

    const mb_cache& nzc_cache_y  = m_context_variables.non_zero_count[to_int(colour_component_e::Y)];
    const mb_cache& nzc_cache_cb = m_context_variables.non_zero_count[to_int(colour_component_e::Cb)];
    const mb_cache& nzc_cache_cr = m_context_variables.non_zero_count[to_int(colour_component_e::Cr)];

    nzc[MB_NZC_DC_BLOCK_IDX_Y] = nzc_cache_y[0];

    std::memcpy(&nzc[0 * 4], &nzc_cache_y[1 * 8 + 4], 4);
    std::memcpy(&nzc[1 * 4], &nzc_cache_y[2 * 8 + 4], 4);
    std::memcpy(&nzc[2 * 4], &nzc_cache_y[3 * 8 + 4], 4);
    std::memcpy(&nzc[3 * 4], &nzc_cache_y[4 * 8 + 4], 4);

    if (m_context_variables.chroma_array_type == 0)
        return;

    nzc[MB_NZC_DC_BLOCK_IDX_Cb] = nzc_cache_cb[0];
    nzc[MB_NZC_DC_BLOCK_IDX_Cr] = nzc_cache_cr[0];

    std::memcpy(&nzc[4 * 4], &nzc_cache_cb[1 * 8 + 4], 4);
    std::memcpy(&nzc[5 * 4], &nzc_cache_cb[2 * 8 + 4], 4);
    std::memcpy(&nzc[8 * 4], &nzc_cache_cr[1 * 8 + 4], 4);
    std::memcpy(&nzc[9 * 4], &nzc_cache_cr[2 * 8 + 4], 4);

    if (m_context_variables.chroma_array_type < 3)
        return;

    std::memcpy(&nzc[ 6 * 4], &nzc_cache_cb[3 * 8 + 4], 4);
    std::memcpy(&nzc[ 7 * 4], &nzc_cache_cb[4 * 8 + 4], 4);
    std::memcpy(&nzc[10 * 4], &nzc_cache_cr[3 * 8 + 4], 4);
    std::memcpy(&nzc[11 * 4], &nzc_cache_cr[4 * 8 + 4], 4);
}

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

        /* Table 9-3 Assignment of syntax element to codeNum for signed Exp-Golomb coded syntax elements se(v) */
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

/**
 * 9.3.3.1.1.9 Derivation process of ctxIdxInc for the syntax element coded_block_flag
 *
 * ctxBlockCat < 5
 * Type of binarization: FL, cMax = 1
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 85..104
 *
 * 5 < ctxBlockCat < 9
 * Type of binarization: FL, cMax = 1
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 460..471
 *
 * 9 < ctxBlockCat < 13
 * Type of binarization: FL, cMax = 1
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 472..483
 *
 * ctxBlockCat = 5 || ctxBlockCat = 9 || ctxBlockCat = 13
 * Type of binarization: FL, cMax = 1
 * maxBinIdxCtx: 0
 * ctxIdxOffset: 1012..1023
 */
int picture_cabac::decode_coded_block_flag(int ctxBlockCat, int idx)
{

    /* Table 9-40 – Assignment of ctxIdxBlockCatOffset to ctxBlockCat for syntax elements coded_block_flag,
       significant_coeff_flag, last_significant_coeff_flag, and coeff_abs_level_minus1 */
    static const int base_ctx[14] = {
        85 + 0, 85 + 4, 85 + 8, 85 + 12, 85 + 16, // ctxBlockCat < 5
        1012,                                     // ctxBlockCat == 5
        460 + 0, 460 + 4, 460 + 8,                // 5 < ctxBlockCat < 9
        1012 + 4,                                 // ctxBlockCat == 9
        472 + 0, 472 + 4, 472 + 8,                // 9 < ctxBlockCat < 13
        1012 + 8                                  // ctxBlockCat == 13
    };

    int nza, nzb;
    int ctxIdxInc = 0;

    if (idx < 16 * COLOUR_COMPONENTS_MAX) { /* ac */
        idx /= COLOUR_COMPONENTS_MAX;
        mb_cache& nzc_cache = m_context_variables.non_zero_count[idx];

        nza = nzc_cache[mb_cache_idx[idx] - 1];
        nzb = nzc_cache[mb_cache_idx[idx] - 8];
    }
    else { /* dc */
        mb* curr_mb = m_context_variables.curr_mb;

        if (curr_mb->left)
            nza = curr_mb->left->non_zero_count[idx];
        else
            nza = MB_IS_INTRA(curr_mb->type);

        if (curr_mb->top)
            nzb = curr_mb->top->non_zero_count[idx];
        else
            nzb = MB_IS_INTRA(curr_mb->type);
    }

    if (nza > 0)
        ctxIdxInc += 1;

    if (nzb > 0)
        ctxIdxInc += 2;

    return m_cabac_decoder.decode_decision(base_ctx[ctxBlockCat] + ctxIdxInc);
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

