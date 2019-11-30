/**
 * @file h264_decoder.hpp
 *
 * ISO/IEC 14496-10/ITU-T H.264 decoder structure.
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

#ifndef _H264_DECODER_HPP_
#define _H264_DECODER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_parser.hpp"
#include "h264_dimensions.hpp"
#include "scaling_matrices.hpp"
#include "slice_header.hpp"
#include "slice_data.hpp"
#include "picture.hpp"
#include "picture_cavlc.hpp"
#include "picture_cabac.hpp"
#include "mb.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define H264_QP_MAX (51 + 2 * 6) // The maximum supported qp

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

class h264_decoder
{
friend class h264::picture;
friend class h264::picture_cavlc;
friend class h264::picture_cabac;

public:
    explicit h264_decoder(h264_parser_container_e container);
    ~h264_decoder();

    void feed(const uint8_t* data, std::size_t count);

    std::string to_string() const
    {
        std::ostringstream stream;

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    const int* get_dequant4x4_table(int table_idx, int qp)
    {
      return (*m_dequant4x4_coeff[table_idx])[qp];
    }

    const int* get_dequant8x8_table(int table_idx, int qp)
    {
      return (*m_dequant8x8_coeff[table_idx])[qp];
    }

private:
    void init_chroma_qp_tables();
    void init_dequant4x4_coeff_table();
    void init_dequant8x8_coeff_table();
    void init_dequantisation_tables();

    void decode_slice(const h264::slice_header& sh, const h264::slice_data& sd);

    void parse();

private:
    h264_parser m_parser;
    h264_dimensions m_dimensions;

    const h264::sps* m_active_sps;
    const h264::pps* m_active_pps;

    h264::picture *m_picture;

    /* dequantisation tables */
    int m_dequant4x4_buffer[SL_4x4_NUM][H264_QP_MAX + 1][16];
    int m_dequant8x8_buffer[SL_8x8_NUM][H264_QP_MAX + 1][64];
    int (*m_dequant4x4_coeff[SL_4x4_NUM])[H264_QP_MAX + 1][16];
    int (*m_dequant8x8_coeff[SL_8x8_NUM])[H264_QP_MAX + 1][64];

    /* pre-scaled (with chroma_qp_index_offset and second_chroma_qp_index_offset)
       version of qp_table */
    uint8_t m_chroma_qp_table[2][64];
};

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

#endif /* _H264_DECODER_HPP_ */
