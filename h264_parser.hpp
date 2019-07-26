/**
 * @file h264_parser.hpp
 *
 * Definition of H.264 (ISO/IEC 14496-10) parser.
 *
 * H.264 syntax and semantics is defined in ISO/IEC 14496-10
 * "Information technology — Coding of audio-visual objects —
 * Part 10: Advanced Video Coding"
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

#ifndef _H264_PARSER_HPP_
#define _H264_PARSER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <sstream>
#include <cstring>
#include <functional> /* for std::function */

#if defined(DEBUG)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "istream.hpp"
#include "base_parser.hpp"

#include "h264_structure.hpp"
#include "aud.hpp"
#include "sps.hpp"
#include "pps.hpp"
#include "sei.hpp"
#include "slice_header.hpp"
#include "slice_data.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define H264_PARSER_CONTAINERS \
    H264_PARSER_CONTAINER(NONE) \
    H264_PARSER_CONTAINER(ANNEX_B) \

#define H264_PARSER_STATUSES \
    H264_PARSER_STATUS(NEED_BYTES) \
    H264_PARSER_STATUS(NAL_UNIT_SKIPPED) \
    H264_PARSER_STATUS(NAL_UNIT_CORRUPTED) \
    H264_PARSER_STATUS(AUD_PARSED) \
    H264_PARSER_STATUS(SPS_PARSED) \
    H264_PARSER_STATUS(PPS_PARSED) \
    H264_PARSER_STATUS(SEI_PARSED) \
    H264_PARSER_STATUS(SLICE_PARSED) \

#define H264_PARSER_STRUCTURES \
    H264_PARSER_STRUCTURE(AUD) \
    H264_PARSER_STRUCTURE(SPS) \
    H264_PARSER_STRUCTURE(PPS) \
    H264_PARSER_STRUCTURE(SEI) \
    H264_PARSER_STRUCTURE(SLICE_HEADER) \
    H264_PARSER_STRUCTURE(SLICE_DATA) \

#define H264_PARSER_MAX_NUMBER_OF_SPS          32
#define H264_PARSER_MAX_NUMBER_OF_PPS         256
#define H264_PARSER_STRUCTURE_ID_RECENT       (-1)
#define H264_PARSER_STRUCTURE_ID_PREVIOUS     (-2)

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

enum class h264_parser_container_e : int32_t
{
#define H264_PARSER_CONTAINER(id) id,
    H264_PARSER_CONTAINERS
#undef H264_PARSER_CONTAINER
};

constexpr static inline const char* to_string(h264_parser_container_e e)
{
    const char *str = "invalid 'h264_parser_container_e' value";

    switch (e) {
#define H264_PARSER_CONTAINER(id) case h264_parser_container_e::id: str = #id; break;
            H264_PARSER_CONTAINERS
#undef H264_PARSER_CONTAINER
    }

    return str;
}

enum class h264_parser_status_e : int32_t
{
#define H264_PARSER_STATUS(id) id,
    H264_PARSER_STATUSES
#undef H264_PARSER_STATUS
};

constexpr static inline const char* to_string(h264_parser_status_e e)
{
    const char *str = "invalid 'parser_status_e' value";

    switch (e) {
#define H264_PARSER_STATUS(id) case h264_parser_status_e::id: str = #id; break;
            H264_PARSER_STATUSES
#undef H264_PARSER_STATUS
    }

    return str;
}

enum class h264_parser_structure_e : int32_t
{
#define H264_PARSER_STRUCTURE(id) id,
    H264_PARSER_STRUCTURES
#undef H264_PARSER_STRUCTURE
};

constexpr static inline const char* to_string(h264_parser_structure_e e)
{
    const char *str = "invalid 'h264_parser_structure_e' value";

    switch (e) {
#define H264_PARSER_STRUCTURE(id) case h264_parser_structure_e::id: str = #id; break;
            H264_PARSER_STRUCTURES
#undef H264_PARSER_STRUCTURE
    }

    return str;
}

class h264_parser : public base_parser<uint8_t>
{
public:
    explicit h264_parser(std::size_t capacity, h264_parser_container_e container) :
        base_parser<uint8_t>(capacity),
        m_parse_function(nullptr),
        m_aud(),
        m_sps_table(),
        m_recent_sps(-1),
        m_pps_table(),
        m_recent_pps(-1),
        m_sei(),
        m_slice_header(),
        m_slice_data()
    {
        switch (container) {
            case h264_parser_container_e::NONE:
                m_parse_function = &h264_parser::parse_nal_units;
                break;

            case h264_parser_container_e::ANNEX_B:
                m_parse_function = &h264_parser::parse_byte_stream_nal_units;
                break;

            default:
                /* do nothing */
                break;
        }

#if defined(DEBUG)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << to_string() << std::endl;
#endif
    }

    virtual ~h264_parser()
    {
#if defined(DEBUG)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    h264_parser(const h264_parser&) = delete;
    h264_parser(h264_parser&&) = delete;
    h264_parser& operator = (const h264_parser&) = delete;
    h264_parser& operator = (h264_parser&&) = delete;

    void reset()
    {
        base_parser::reset();
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << base_parser::to_string();
        stream << std::endl;
        stream << "h264_parser@";
        stream << std::hex << this;

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    h264_parser_status_e parse()
    {
        return (this->*m_parse_function)();
    }

    /**
     * Gives pointer to the requested structure.
     *
     * @param[in] structure The type of the requested structure.
     * @param[in] id Additional identifier of the structure.
     *               Valid only for sps, pps and slice headers.
     *               If H264_PARSER_STRUCTURE_ID_RECENT is passed,
     *               then recently parsed structure is provided in return value.
     *               If H264_PARSER_STRUCTURE_ID_PREVIOUS is passed
     *               (valid only for slice headers),
     *               then previously (the one before recent) parsed structure
     *               is provided in return value.
     *
     * @return Pointer to the requested struture. You must cast it to proper type.
     */
    const h264_structure* get_structure(h264_parser_structure_e structure, int id);

private:
    using parse_function_t = h264_parser_status_e (h264_parser::*)();

    /**
     * Finds 'next_start_code (0x000001)' pattern in parser's flatbuffer.
     *
     * @return true when next_start_code has been found in parser's flatbuffer,
     *         false otherwise.
     *
     * @note Annex B Byte stream format
     */
    bool find_next_start_code();

    /**
     * Finds next NAL (Network Abstraction Layer) unit in parser's flatbuffer.
     *
     * @param[in,out] parser Handle to the parser's instance.
     *
     * @return Number of bytes composing nal unit (NumBytesInNALunit),
     *         0 if start of a nal unit could not be found,
     *        -1 if end of a nal unit could not be found.
     */
    int find_nal_unit();

    void parse_scaling_list_4x4(istream_be& s, h264::scaling_list_4x4 (&lists)[SL_4x4_NUM], int list);
    void parse_scaling_list_8x8(istream_be& s, h264::scaling_list_8x8 (&lists)[SL_8x8_NUM], int list);
    void parse_scaling_matrices_4x4(istream_be& s, h264::scaling_matrices& sm);
    void parse_scaling_matrices_8x8(istream_be& s, h264::scaling_matrices& sm, uint32_t chroma_format_idc);
    bool parse_scaling_matrices(istream_be& s, h264::scaling_matrices& sm, bool parse8x8, uint32_t chroma_format_idc);

    bool parse_hrd_parameters(istream_be& s, h264::hrd_parameters& hrd);
    bool parse_vui_parameters(istream_be& s, h264::vui_parameters& vui);

    bool parse_ref_pic_list_modification(istream_be& s, h264::ref_pic_list_modification& rplm);
    bool parse_pred_weight_table(istream_be& s, h264::pred_weight_table& pwt);
    bool parse_dec_ref_pic_marking_idr(istream_be& s, h264::dec_ref_pic_marking_idr& drpm);
    bool parse_dec_ref_pic_marking_nonidr(istream_be& s, h264::dec_ref_pic_marking_nonidr& drpm);

    h264_parser_status_e parse_aud(istream_be& s);
    h264_parser_status_e parse_sps(istream_be& s);
    h264_parser_status_e parse_pps(istream_be& s);
    h264_parser_status_e parse_sei(istream_be& s);
    h264_parser_status_e parse_slice_header(istream_be& s, uint32_t nal_ref_idc, uint32_t nal_unit_type);
    h264_parser_status_e parse_nal_unit(istream_be& s);

    h264_parser_status_e parse_nal_units();
    h264_parser_status_e parse_byte_stream_nal_units();

    void set_slice_data(istream_be& s);

    parse_function_t m_parse_function;

    h264::aud m_aud;

    h264::sps  m_sps_table[H264_PARSER_MAX_NUMBER_OF_SPS];
    int        m_recent_sps;

    h264::pps  m_pps_table[H264_PARSER_MAX_NUMBER_OF_PPS];
    int        m_recent_pps;

    h264::sei m_sei;

    h264::slice_header m_slice_header;
    h264::slice_data m_slice_data;
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

#endif /* _H264_PARSER_HPP_ */
