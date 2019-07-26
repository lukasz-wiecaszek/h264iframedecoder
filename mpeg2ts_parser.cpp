/**
 * @file mpeg2ts_parser.cpp
 *
 * Brief description of what's inside this source file.
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
#include "mpeg2ts_pid.hpp"
#include "mpeg2ts_parser.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace ymn;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define MPEG2TS_PARSER_SYNC_BYTE                     0x47
#define MPEG2TS_PARSER_FORWARD_SCAN                    10
#define MPEG2TS_PARSER_NO_SYNC_INDICATOR               -1

#define MPEG2TS_PARSER_PACKET_SIZE_EXTENSIONS \
    X(REED_SOLOMON_CODE_LENGTH_DVB,  16) \
    X(REED_SOLOMON_CODE_LENGTH_ATSC, 20) \

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

enum mpeg2ts_parser_packet_size_extensions
{
#define X(a, b) a = b,
    MPEG2TS_PARSER_PACKET_SIZE_EXTENSIONS
#undef X
};

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
static constexpr std::size_t mpeg2ts_parser_packet_sizes[] = {
    MPEG2TS_PARSER_PACKET_SIZE,
#define X(a, b) MPEG2TS_PARSER_PACKET_SIZE + a,
    MPEG2TS_PARSER_PACKET_SIZE_EXTENSIONS
#undef X
};

static constexpr std::size_t mpeg2ts_parser_packet_sizes_elements =
    sizeof(mpeg2ts_parser_packet_sizes) / sizeof(mpeg2ts_parser_packet_sizes[0]);

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
static constexpr std::size_t mpeg2ts_parser_bigest_packet_size()
{
    std::size_t max = 0;

    for (std::size_t n = 0; n < mpeg2ts_parser_packet_sizes_elements; ++n)
        if (mpeg2ts_parser_packet_sizes[n] > max)
            max = mpeg2ts_parser_packet_sizes[n];

    return max;
}

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/
int mpeg2ts_parser::synchronize(const std::size_t tspacket_size)
{
    int offset;
    std::size_t i, j;
    const uint8_t *p;

    offset = MPEG2TS_PARSER_NO_SYNC_INDICATOR;
    p = m_flatbuffer.read_ptr();

    for (i = 0;
        ((i < tspacket_size) && (MPEG2TS_PARSER_NO_SYNC_INDICATOR == offset));
        ++i) {
        if (p[i] != MPEG2TS_PARSER_SYNC_BYTE) continue;
        offset = static_cast<int>(i);
        for (j = 1; j <= MPEG2TS_PARSER_FORWARD_SCAN; ++j) {
            if (p[j * tspacket_size + offset] != MPEG2TS_PARSER_SYNC_BYTE) {
                offset = MPEG2TS_PARSER_NO_SYNC_INDICATOR;
                break;
            }
        }
    }

    return offset;
}

mpeg2ts_parser_status_e mpeg2ts_parser::waiting_for_sync()
{
    mpeg2ts_parser_status_e status = mpeg2ts_parser_status_e::NOT_SYNCHRONIZED;

    for (std::size_t n = 0; n < mpeg2ts_parser_packet_sizes_elements; ++n) {
        int offset;
        std::size_t packet_size = mpeg2ts_parser_packet_sizes[n];

        if (m_flatbuffer.read_available() < (packet_size * (MPEG2TS_PARSER_FORWARD_SCAN + 1))) {
            status = mpeg2ts_parser_status_e::NEED_BYTES;
            break;
        }

        offset = synchronize(packet_size);
        if (MPEG2TS_PARSER_NO_SYNC_INDICATOR != offset) {
            m_flatbuffer.consume(offset);
            m_state_function = &mpeg2ts_parser::synchronized;
            m_tspacket_size = packet_size;
            status = mpeg2ts_parser_status_e::SYNC_GAINED;
            break;
        }
    }

    if (mpeg2ts_parser_status_e::NOT_SYNCHRONIZED == status)
        m_flatbuffer.consume(MPEG2TS_PARSER_PACKET_SIZE);

    return status;
}

mpeg2ts_parser_status_e mpeg2ts_parser::synchronized()
{
    if (m_flatbuffer.read_available() < m_tspacket_size)
        return mpeg2ts_parser_status_e::NEED_BYTES;

    mpeg2ts_parser_status_e status = mpeg2ts_parser_status_e::SYNCHRONIZED;

    const uint8_t *tsp = m_flatbuffer.read_ptr();
    if (MPEG2TS_PARSER_SYNC_BYTE == *tsp) {
        if (!m_transport_rate.is_valid())
            if (m_transport_rate.calculate(tsp))
                status = mpeg2ts_parser_status_e::TRANSPORT_RATE_DETECTED;
    }
    else {
        m_state_function = &mpeg2ts_parser::waiting_for_sync;
        m_tspacket_size = 0;
        m_transport_rate.reset();
        status = mpeg2ts_parser_status_e::SYNC_LOST;
    }

    return status;
}

bool mpeg2ts_parser::transport_rate::calculate(const uint8_t* tsp)
{
    bool status = false;

    if (0 == m_transport_rate) {
        mpeg2ts_pid pid = mpeg2ts_parser_get_pid(tsp);

        if (MPEG2TS_PID_INVALID == m_pcr_pid) {
           if (mpeg2ts_parser_has_pcr(tsp)) { /* 1st pcr */
              m_pcr1 = mpeg2ts_parser_get_pcr(tsp);
              m_pcr_pid = pid;
           }
        }
        else {
           m_no_of_packets++;
           if (pid == m_pcr_pid) {
              if (mpeg2ts_parser_has_pcr(tsp)) { /* 2nd pcr */
                 m_pcr2 = mpeg2ts_parser_get_pcr(tsp);
                 if (m_pcr2 > m_pcr1) {
                     m_transport_rate = m_no_of_packets * 27000000 /
                         (m_pcr2 - m_pcr1);
                     status = true;
                 }
                 else {
                     m_pcr1 = m_pcr2;
                     m_no_of_packets = 0;
                 }
              }
           }
        }
    }
    else
        status = true;

    return status;
}

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/

