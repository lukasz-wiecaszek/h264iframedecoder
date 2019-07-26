/**
 * @file h264_decoder.cpp
 *
 * ISO/IEC 14496-10/ITU-T H.264 decoder.
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
#include <cstdint>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_decoder.hpp"
#include "h264_parser.hpp"
#include "h264_cabac_decoder.hpp"
#include "h264_dimensions.hpp"
#include "scaling_matrices.hpp"
#include "slice_header.hpp"
#include "slice_data.hpp"
#include "picture_cavlc.hpp"
#include "picture_cabac.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace ymn;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define H264_PARSER_BUFFER_SIZE  (4 * 1024 * 1024)

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
h264_decoder::h264_decoder(h264_parser_container_e container) :
    m_parser{H264_PARSER_BUFFER_SIZE, container},
    m_dimensions{},
    m_active_sps{nullptr},
    m_active_pps{nullptr},
    m_picture{nullptr}
{
}

h264_decoder::~h264_decoder()
{
}

void h264_decoder::feed(const uint8_t* data, std::size_t count)
{
    size_t n_written;

    do {
        n_written = m_parser.write(data, count);
        if (0 == n_written) {
            m_parser.reset();
            continue;
        }

        if (n_written > count) {
            std::cerr << "error: parser swallowed more data then requested (this is really bad)" << std::endl;
            m_parser.reset();
            continue;
        }

        parse();

        data += n_written;
        count -= n_written;
    } while (count > 0);
}

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/
/* 8.5.8 Derivation process for chroma quantisation parameters */
void h264_decoder::init_chroma_qp_tables()
{
    const int depth = m_active_sps->bit_depth_luma_minus8;
    const int max_qp = 51 + 6 * depth;
    int offset;

#define QP(qp, depth) ((qp) + 6 * (depth))
#define CHROMA_QP_TABLE_END(d)                                        \
    QP(0,  d), QP(1,  d), QP(2,  d), QP(3,  d), QP(4,  d), QP(5,  d), \
    QP(6,  d), QP(7,  d), QP(8,  d), QP(9,  d), QP(10, d), QP(11, d), \
    QP(12, d), QP(13, d), QP(14, d), QP(15, d), QP(16, d), QP(17, d), \
    QP(18, d), QP(19, d), QP(20, d), QP(21, d), QP(22, d), QP(23, d), \
    QP(24, d), QP(25, d), QP(26, d), QP(27, d), QP(28, d), QP(29, d), \
    QP(29, d), QP(30, d), QP(31, d), QP(32, d), QP(32, d), QP(33, d), \
    QP(34, d), QP(34, d), QP(35, d), QP(35, d), QP(36, d), QP(36, d), \
    QP(37, d), QP(37, d), QP(37, d), QP(38, d), QP(38, d), QP(38, d), \
    QP(39, d), QP(39, d), QP(39, d), QP(39, d)

    /* One chroma qp table for each supported bit depth (8, 9, 10). */
    static const uint8_t chroma_qp[3][H264_QP_MAX + 1] =
    {
        {CHROMA_QP_TABLE_END(0)},
        {0, 1, 2, 3, 4, 5, CHROMA_QP_TABLE_END(1)},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, CHROMA_QP_TABLE_END(2)}
    };

    offset = m_active_pps->chroma_qp_index_offset;
    for (int q = 0; q < max_qp + 1; ++q)
        m_chroma_qp_table[0][q] = chroma_qp[depth][std::clamp(q + offset, 0, max_qp)];

    offset = m_active_pps->second_chroma_qp_index_offset;
    for (int q = 0; q < max_qp + 1; ++q)
        m_chroma_qp_table[1][q] = chroma_qp[depth][std::clamp(q + offset, 0, max_qp)];

#undef CHROMA_QP_TABLE_END
#undef QP
}

/* 8.5.9 Derivation process for scaling functions */
void h264_decoder::init_dequant4x4_coeff_table()
{
    int i, j;
    const int depth = m_active_sps->bit_depth_luma_minus8;
    const int max_qp = 51 + 6 * depth;

    static const uint8_t v[6][3] =
    {
        {10, 13, 16},
        {11, 14, 18},
        {13, 16, 20},
        {14, 18, 23},
        {16, 20, 25},
        {18, 23, 29},
    };

    for (i = 0; i < SL_4x4_NUM; ++i) {
        m_dequant4x4_coeff[i] = &m_dequant4x4_buffer[i];
        for (j = 0; j < i; ++j) {
            if (0 == std::memcmp(m_active_pps->sm.scaling_matrices_4x4[j].scaling_list,
                                 m_active_pps->sm.scaling_matrices_4x4[i].scaling_list, 16)) {
                m_dequant4x4_coeff[i] = &m_dequant4x4_buffer[j];
                break;
            }
        }
        if (j < i)
            continue;

        for (int q = 0; q < max_qp + 1; ++q) {
            int shift = q / 6;
            int idx   = q % 6;
            for (int x = 0; x < 16; ++x) {
                uint8_t idx2 = (x & 1) + ((x >> 2) & 1);
                uint8_t level_scale_4x4 =
                    m_active_pps->sm.scaling_matrices_4x4[i].scaling_list[x] * v[idx][idx2];
                (*m_dequant4x4_coeff[i])[q][x] = level_scale_4x4 << shift;
            }
        }
    }
}

/* 8.5.9 Derivation process for scaling functions */
void h264_decoder::init_dequant8x8_coeff_table()
{
    int i, j;
    const int depth = m_active_sps->bit_depth_luma_minus8;
    const int max_qp = 51 + 6 * depth;

    static const uint8_t v[6][6] =
    {
        {20, 18, 32, 19, 25, 24},
        {22, 19, 35, 21, 28, 26},
        {26, 23, 42, 24, 33, 31},
        {28, 25, 45, 26, 35, 33},
        {32, 28, 51, 30, 40, 38},
        {36, 32, 58, 34, 46, 43},
    };

    static const uint8_t v_scan[16] =
    {
        0, 3, 4, 3, 3, 1, 5, 1, 4, 5, 2, 5, 3, 1, 5, 1
    };

    for (i = 0; i < SL_8x8_NUM; ++i) {
        m_dequant8x8_coeff[i] = &m_dequant8x8_buffer[i];
        for (j = 0; j < i; ++j) {
            if (0 == std::memcmp(m_active_pps->sm.scaling_matrices_8x8[j].scaling_list,
                                 m_active_pps->sm.scaling_matrices_8x8[i].scaling_list, 64)) {
                m_dequant8x8_coeff[i] = &m_dequant8x8_buffer[j];
                break;
            }
        }
        if (j < i)
            continue;

        for (int q = 0; q < max_qp + 1; ++q) {
            int shift = q / 6;
            int idx   = q % 6;
            for (int x = 0; x < 64; ++x) {
                uint8_t idx2 = v_scan[((x >> 1) & 12) | (x & 3)];
                uint8_t level_scale_8x8 =
                    m_active_pps->sm.scaling_matrices_8x8[i].scaling_list[x] * v[idx][idx2];
                (*m_dequant8x8_coeff[i])[q][x] = level_scale_8x8 << shift;
            }
        }
    }
}

void h264_decoder::init_dequantisation_tables()
{
    init_dequant4x4_coeff_table();
    if (m_active_pps->transform_8x8_mode_flag)
        init_dequant8x8_coeff_table();

    if (m_active_sps->qpprime_y_zero_transform_bypass_flag) {
        for (int i = 0; i < SL_4x4_NUM; ++i)
            for (int x = 0; x < 16; ++x)
                (*m_dequant4x4_coeff[i])[0][x] = 1 << 6;

        if (m_active_pps->transform_8x8_mode_flag)
            for (int i = 0; i < SL_8x8_NUM; ++i)
                for (int x = 0; x < 64; ++x)
                    (*m_dequant8x8_coeff[i])[0][x] = 1 << 6;
    }
}

void h264_decoder::decode_slice(const h264::slice_header& sh, const h264::slice_data& sd)
{
    bool sps_changed = false;
    bool pps_changed = false;

    const h264::pps* active_pps = sh.get_active_pps();

    if (nullptr == active_pps)
        return;

    const h264::sps* active_sps = active_pps->get_active_sps();

    if (nullptr == active_sps)
        return;

    if (nullptr == m_active_sps ||
        m_active_sps->seq_parameter_set_id != active_sps->seq_parameter_set_id) {
        m_active_sps = active_sps;
        sps_changed = true;
    }

    if (nullptr == m_active_pps ||
        m_active_pps->pic_parameter_set_id != active_pps->pic_parameter_set_id) {
        m_active_pps = active_pps;
        pps_changed = true;
    }

    if (sps_changed) {
        /* active sps has changed, so reinit dimensions */
        m_dimensions.reset(*m_active_sps);
        std::cout << std::endl << m_dimensions.to_string();
    }

    if (pps_changed) {
        /* active pps has changed, so reinit:
           - dequantisation tables
           - chroma_qp_tables
        */
        init_dequantisation_tables();
        init_chroma_qp_tables();
    }

    if ((sh.slice_type == h264::slice_type_e::I) || (sh.slice_type == h264::slice_type_e::SI)) {
        /* entropy_coding_mode_flag selects the entropy decoding method to be applied
        for the syntax elements for which two descriptors appear in the syntax tables as follows.
        - If entropy_coding_mode_flag is equal to 0, the method specified by the left
          descriptor in the syntax table is applied (Exp-Golomb coded, see subclause 9.1
          or CAVLC, see subclause 9.2).
        - Otherwise (entropy_coding_mode_flag is equal to 1), the method specified by
          the right descriptor in the syntax table is applied (CABAC, see subclause 9.3). */
        if (m_active_pps->entropy_coding_mode_flag)
            m_picture = new h264::picture_cabac(*this, sh);
        else
            m_picture = new h264::picture_cavlc(*this, sh);

        m_picture->decode(sh, sd);

        delete m_picture;
    }
}

void h264_decoder::parse()
{
    h264_parser_status_e status;

    do {
        status = m_parser.parse();

        switch (status) {
            case h264_parser_status_e::NAL_UNIT_SKIPPED:
                break;

            case h264_parser_status_e::NAL_UNIT_CORRUPTED:
                break;

            case h264_parser_status_e::AUD_PARSED:
            {
                const h264_structure* aud;

                aud = m_parser.get_structure(
                    h264_parser_structure_e::AUD,
                    H264_PARSER_STRUCTURE_ID_RECENT);

                if (aud)
                    std::cout << aud->to_string();
                else
                    std::cout << "Parser returned '" << ymn::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            case h264_parser_status_e::SPS_PARSED:
            {
                const h264_structure* sps;

                sps = m_parser.get_structure(
                    h264_parser_structure_e::SPS,
                    H264_PARSER_STRUCTURE_ID_RECENT);

                if (sps)
                    std::cout << sps->to_string();
                else
                    std::cout << "Parser returned '" << ymn::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            case h264_parser_status_e::PPS_PARSED:
            {
                const h264_structure* pps;

                pps = m_parser.get_structure(
                    h264_parser_structure_e::PPS,
                    H264_PARSER_STRUCTURE_ID_RECENT);

                if (pps)
                    std::cout << pps->to_string();
                else
                    std::cout << "Parser returned '" << ymn::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            case h264_parser_status_e::SEI_PARSED:
            {
                const h264_structure* sei;

                sei = m_parser.get_structure(
                    h264_parser_structure_e::SEI,
                    H264_PARSER_STRUCTURE_ID_RECENT);

                if (sei)
                    std::cout << sei->to_string();
                else
                    std::cout << "Parser returned '" << ymn::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;
                break;
            }

            case h264_parser_status_e::SLICE_PARSED:
            {
                const h264_structure* slice_header;

                slice_header = m_parser.get_structure(
                    h264_parser_structure_e::SLICE_HEADER,
                    H264_PARSER_STRUCTURE_ID_RECENT);

                if (slice_header)
                    std::cout << slice_header->to_string();
                else
                    std::cout << "Parser returned '" << ymn::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;

                const h264_structure* slice_data;

                slice_data = m_parser.get_structure(
                    h264_parser_structure_e::SLICE_DATA,
                    H264_PARSER_STRUCTURE_ID_RECENT);

                if (slice_data)
                    std::cout << slice_data->to_string();
                else
                    std::cout << "Parser returned '" << ymn::to_string(status) <<
                        "' but associated structure cannot be retrived" << std::endl;

                if (slice_header && slice_data)
                    decode_slice(
                        *reinterpret_cast<const h264::slice_header*>(slice_header),
                        *reinterpret_cast<const h264::slice_data*>(slice_data));
                break;
            }

            default:
                break;
       }
    } while (status != h264_parser_status_e::NEED_BYTES);
}

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/

