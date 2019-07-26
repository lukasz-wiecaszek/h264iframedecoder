/**
 * @file h264_cabac_decoder.hpp
 *
 * ISO/IEC 14496-10/ITU-T H.264 cabac decoder.
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

#ifndef _H264_CABAC_DECODER_HPP_
#define _H264_CABAC_DECODER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdint>
#include <array>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "slice_header.hpp"
#include "slice_data.hpp"
#include "istream.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define NUMBER_OF_CONTEXT_VARIABLES 1024

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

class h264_cabac_decoder
{
public:
    explicit h264_cabac_decoder();

    // The process of initialization of context variables is described in chapter 9.3.1.1
    // "Initialisation process for context variables" of ISO/IEC 14496-10/ITU-T H.264.
    void init_context_variables(const h264::slice_header& slice_header, int32_t slice_qp);

    // The process of initialization of decoding engine is described in chapter 9.3.1.2
    // "Initialisation process for the arithmetic decoding engine" of ISO/IEC 14496-10/ITU-T H.264.
    void init_decoding_engine(const h264::slice_data& slice_data);

    int decode_bypass();
    int decode_terminate();
    int decode_decision(int ctxIdx);

private:
    struct context_variable
    {
       int pStateIdx;
       int valMPS;
    };

    istream_be m_stream;
    uint32_t m_codIRange;
    uint32_t m_codIOffset;

    std::array<context_variable, NUMBER_OF_CONTEXT_VARIABLES> m_context_variables;
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

#endif /* _H264_CABAC_DECODER_HPP_ */
