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

class picture_cabac : public picture
{
public:
    explicit picture_cabac(const h264_decoder& decoder, const h264::slice_header& sh);
    ~picture_cabac() override;

    void decode(const h264::slice_header& sh, const h264::slice_data& sd) override;

private:
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
    int decode_coeff_abs_level_minus1();

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
