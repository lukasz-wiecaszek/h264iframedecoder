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
    mb *mb;

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

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/

