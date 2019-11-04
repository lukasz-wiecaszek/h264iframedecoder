/**
 * @file picture_cabac.hpp
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

#ifndef _PICTURE_CABAC_HPP_
#define _PICTURE_CABAC_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "picture.hpp"
#include "h264_definitions.hpp"
#include "h264_cabac_decoder.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

namespace h264
{

enum ctx_block_cat_e
{
    CAT_16x16_DC_Y    = 0,
    CAT_16x16_AC_Y    = 1,
    CAT_4x4_Y         = 2,
    CAT_CHROMA_DC     = 3,
    CAT_CHROMA_AC     = 4,
    CAT_8x8_Y         = 5,
    CAT_16x16_DC_Cb   = 6,
    CAT_16x16_AC_Cb   = 7,
    CAT_4x4_Cb        = 8,
    CAT_8x8_Cb        = 9,
    CAT_16x16_DC_Cr   = 10,
    CAT_16x16_AC_Cr   = 11,
    CAT_4x4_Cr        = 12,
    CAT_8x8_Cr        = 13,
    CAT_NUM           = 14
};

class picture_cabac : public picture
{
public:
    explicit picture_cabac(const h264_decoder& decoder, const h264::slice_header& sh);
    ~picture_cabac() override;

    void decode(const h264::slice_header& sh, const h264::slice_data& sd) override;

private:
    void intraNxN_pred_mode_cache_init(uint32_t constrained_intra_pred_flag);
    int get_predicted_intra_mode(int idx);
    int get_intra4x4_pred_mode(int pred_mode);
    int get_intra8x8_pred_mode(int pred_mode);

    void non_zero_count_cache_init(uint32_t mb_type);
    void non_zero_count_save();

    int decode_mb_field_decoding_flag();
    int decode_mb_type_si_slice();
    int decode_mb_type_i_slice();
    int decode_transform_size_8x8_flag();
    int decode_cbp_luma();
    int decode_cbp_chroma();
    int decode_mb_qp_delta();
    int decode_prev_intra4x4_pred_mode_flag();
    int decode_rem_intra4x4_pred_mode();
    int decode_prev_intra8x8_pred_mode_flag();
    int decode_rem_intra8x8_pred_mode();
    int decode_intra_chroma_pred_mode();
    int decode_coded_block_flag(int ctxBlockCat, int idx);
    int decode_significant_coeff_flag(int ctxBlockCat, int ctxIdxInc);
    int decode_last_significant_coeff_flag(int ctxBlockCat, int ctxIdxInc);
    int decode_coeff_abs_level_minus1(int ctxBlockCat, int ctxIdxInc);

    void decode_residual_block(dctcoeff& block,
                               const enum ctx_block_cat_e cat,
                               const int idx,
                               const uint8_t* scantable,
                               const int max_coeff);

    void decode_residual_dc(dctcoeff& block,
                            const enum ctx_block_cat_e cat,
                            const int idx,
                            const uint8_t* scantable,
                            const int max_coeff);

    void decode_residual_ac(dctcoeff& block,
                            const enum ctx_block_cat_e cat,
                            const int idx,
                            const uint8_t* scantable,
                            const int max_coeff);

    void decode_residual(const uint8_t* scan4x4,
                         const uint8_t* scan8x8,
                         colour_component_e cc);

    void decode_residual();
    void decode_mb(const h264::slice_header& sh);

private:
    h264_cabac_decoder m_cabac_decoder;
};

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

#endif /* _PICTURE_CABAC_HPP_ */
