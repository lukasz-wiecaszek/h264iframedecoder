/**
 * @file h264iframedecoder.cpp
 *
 * ...
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
#include <iostream>
#include <fstream>

#include <cstdlib>
#include <cassert>

extern "C" {
    #include <unistd.h>
    #include <getopt.h>
}

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "utilities.hpp"
#include "strtointeger.hpp"
#include "mpeg2ts_pid.hpp"
#include "mpeg2ts_parser.hpp"
#include "h264_parser.hpp"
#include "h264_decoder.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define READ_BUFFER_SIZE                       (4 * 1024)
#define TS_PARSER_BUFFER_SIZE                  (2 * READ_BUFFER_SIZE)
#define H264_PARSER_BUFFER_SIZE                (4 * 1024 * 1024)

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

struct mpeg2ts_parser_user_data
{
    ymn::mpeg2ts_pid pid;
    ymn::h264_decoder* h264_decoder;
};

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/
static void h264_decoder_feed(ymn::h264_decoder& decoder, const uint8_t* data, std::size_t count);

static void mpeg2ts_parser_demux(ymn::mpeg2ts_parser& parser, const uint8_t *tspayload, std::size_t count, bool payload_unit_start_indicator);
static void mpeg2ts_parser_handle_tspacket(ymn::mpeg2ts_parser& parser, const uint8_t *tspacket);

static void mpeg2ts_parser_parse(ymn::mpeg2ts_parser& parser);
static void mpeg2ts_parser_feed(ymn::mpeg2ts_parser& parser, const uint8_t* data, std::size_t count);

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/
static ymn::h264_decoder* h264_decoder = nullptr;
static ymn::mpeg2ts_parser* mpeg2ts_parser = nullptr;
static std::ofstream h264_ofile;

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
static inline void h264iframedecoder_usage(const char* progname)
{
    std::cout << "usage: " << progname << " [-r] [-t pid] [-a] [-o ofile] <filename>" << std::endl;
    std::cout << " options: " << std::endl;
    std::cout << "  -r --rtp                : Specifies that input h264 stream is additionally encapsulated by" << std::endl;
    std::cout << "                          : RTP Payload Format for H.264 Video (RFC 6184)." << std::endl;
    std::cout << "                          : When used together with -t, then set of ts packets is encapsulated by just one" << std::endl;
    std::cout << "                          : rtp fixed header (RFC 3550 A Transport Protocol for Real-Time Applications)." << std::endl;
    std::cout << std::endl;
    std::cout << "  -t pid --ts=pid         : Specifies that input h264 stream is encapsulated by ISO/IEC 13818-1 transport stream." << std::endl;
    std::cout << "                          : Then pid value shall specify ts packets carrying desired video stream." << std::endl;
    std::cout << std::endl;
    std::cout << "  -a --annex-b            : Specifies that input h264 stream is encapsulated by ISO/IEC 14496-10 Annex B." << std::endl;
    std::cout << "                          : When -t is used this option will be selected automatically." << std::endl;
    std::cout << std::endl;
    std::cout << "  -o ofile --output=ofile : When this option is provided, then selected h264 stream" << std::endl;
    std::cout << "                          : will additionally be stored in file depicted by ofile." << std::endl;
}

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
int main(int argc, char *argv[])
{
    bool status;
    struct {
        bool rtp;
        bool ts;
        bool annex_b;
    } encapsulation = {};
    uint16_t pid = MPEG2TS_PID_INVALID;
    const char* ofile = nullptr;
    ymn::h264_parser_container_e container = ymn::h264_parser_container_e::NONE;
    mpeg2ts_parser_user_data mpeg2ts_user_data;

    static struct option long_options[] = {
        {"rtp",     no_argument,       0, 'r'},
        {"ts",      required_argument, 0, 't'},
        {"annex-b", no_argument,       0, 'a'},
        {"ofile",   required_argument, 0, 'o'},
        {0,         0,                 0,  0 }
    };

    for (;;) {
        int c = getopt_long(argc, argv, "rt:ao:", long_options, 0);
        if (-1 == c)
            break;

        switch(c) {
            case 'r':
                encapsulation.rtp = true;
                break;

            case 't':
                encapsulation.ts = true;
                encapsulation.annex_b = true;
                container = ymn::h264_parser_container_e::ANNEX_B;
                status = (ymn::strtointeger_conversion_status_e::success == ymn::strtointeger(optarg, pid));
                if (!status) {
                    std::cerr << "error: cannot convert '" << optarg << "' to integer" << std::endl;
                    h264iframedecoder_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'a':
                encapsulation.annex_b = true;
                container = ymn::h264_parser_container_e::ANNEX_B;
                break;

            case 'o':
                ofile = optarg;
                break;

            default:
                std::cout << "default option received" << std::endl;
                /* does nothing */
                break;
        }
    }

    std::cout << "encapsulation:"
        << " rtp=" << (encapsulation.rtp ? "y" : "n")
        << " ts=" << (encapsulation.ts ? "y" : "n")
        << " annex-b=" << (encapsulation.annex_b ? "y" : "n")
        << std::endl;
    if (encapsulation.ts)
        std::cout << "pid: " << HEXDEC(pid) << std::endl;
    std::cout << "container: " << to_string(container) << std::endl;

    const char* filename = argv[optind];
    if (!filename) {
         std::cerr << "error: missing input file" << std::endl;
         h264iframedecoder_usage(argv[0]);
         exit(EXIT_FAILURE);
    }

   if (ofile != nullptr) {
      h264_ofile = std::ofstream(ofile, std::ios::out | std::ios::binary);
      if (!h264_ofile.is_open()) {
         std::cerr << "error: could not open '" << ofile << "'" << std::endl;
         h264iframedecoder_usage(argv[0]);
         exit(EXIT_FAILURE);
      }
   }

    h264_decoder = new ymn::h264_decoder(container);
    assert(h264_decoder != nullptr);

    if (encapsulation.ts) {
        mpeg2ts_parser = new ymn::mpeg2ts_parser(TS_PARSER_BUFFER_SIZE);
        assert(mpeg2ts_parser != nullptr);

        mpeg2ts_user_data.pid = pid;
        mpeg2ts_user_data.h264_decoder = h264_decoder;

        mpeg2ts_parser->set_user_data(&mpeg2ts_user_data);
    }

    if (encapsulation.rtp) {
    }

    std::size_t read_bytes = 0;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        std::streamsize count;
        char* memblock;

        memblock = new char[READ_BUFFER_SIZE];

        do {
            count = file.readsome(memblock, READ_BUFFER_SIZE);
            read_bytes += count;
            if (count > 0) {
                if (encapsulation.rtp)
                    /* to be implemented */;
                else
                if (encapsulation.ts)
                    mpeg2ts_parser_feed(*mpeg2ts_parser, reinterpret_cast<uint8_t*>(memblock), count);
                else
                    h264_decoder_feed(*h264_decoder, reinterpret_cast<uint8_t*>(memblock), count);
            }
        } while (count > 0);

        file.close();
        std::cout << "read " << read_bytes << " bytes from '" << filename << "'" << std::endl;
    }
    else {
        std::cerr << "error: could not open '" << filename << "'" << std::endl;
    }

    if (mpeg2ts_parser)
        delete mpeg2ts_parser;

    if (h264_decoder)
        delete h264_decoder;

    if (h264_ofile.is_open())
        h264_ofile.close();

    return 0;
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
static void h264_decoder_feed(ymn::h264_decoder& decoder, const uint8_t* data, std::size_t count)
{
//{
//    const uint8_t *p = data;
//    fprintf(stdout, " 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
//}

    if (h264_ofile.is_open())
        h264_ofile.write(reinterpret_cast<const char*>(data), count);

    decoder.feed(data, count);
}

static void mpeg2ts_parser_demux(ymn::mpeg2ts_parser& parser, const uint8_t *tspayload, std::size_t count, bool payload_unit_start_indicator)
{
    static enum {DMX_IDLE, DMX_HEADER, DMX_DATA} dmx_state = DMX_IDLE;
    static std::size_t buffer_bytes = 0;
    static uint8_t buffer[256];

    mpeg2ts_parser_user_data* mpeg2ts_user_data =
        static_cast<mpeg2ts_parser_user_data*>(parser.get_user_data());

    const uint8_t* p;
    std::size_t available;
    std::size_t len;

#define NEEDBYTES(x)                                                   \
    do {                                                               \
        int missing;                                                   \
                                                                       \
        missing = (x) - available;                                     \
        if (missing > 0) {                                             \
            if (0 == buffer_bytes) {                                   \
                memcpy(buffer, p, available);                          \
                buffer_bytes = available;                              \
                return;                                                \
            }                                                          \
            else {                                                     \
                if ((unsigned int)missing <= count) {                  \
                    memcpy(buffer + buffer_bytes, tspayload, missing); \
                    buffer_bytes += missing;                           \
                    tspayload += missing;                              \
                    count -= missing;                                  \
                    available = (x);                                   \
                }                                                      \
                else {                                                 \
                    memcpy(buffer + buffer_bytes, tspayload, count);   \
                    buffer_bytes += count;                             \
                    return;                                            \
                }                                                      \
            }                                                          \
        }                                                              \
    } while (0)

    if (payload_unit_start_indicator) {
        dmx_state = DMX_HEADER;
        buffer_bytes = 0;
    }

    if (0 == buffer_bytes) {
        p = tspayload;
        available = count;
    }
    else {
        p = buffer;
        available = buffer_bytes;
    }

    switch (dmx_state) {
        case DMX_IDLE:
            return;

        case DMX_HEADER:
            NEEDBYTES(7);

//fprintf(stdout, "0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6]);

            /* 1110 xxxx - 13818-2 or 11172-2 video stream number xxxxx */
            if ((p[0] != 0x00) || (p[1] != 0x00) || (p[2] != 0x01) ||
                (p[3] <  0xe0) || (p[3] >  0xef) || ((p[6] & 0xc0) != 0x80 /* mpeg2 */)) {
                dmx_state = DMX_IDLE;
                return;
            }

            NEEDBYTES(9);
            len = 9 + p[8];
            NEEDBYTES(len);

            if (0 == buffer_bytes) {
                tspayload += len;
                count -= len;
            }

            h264_decoder_feed(*mpeg2ts_user_data->h264_decoder, tspayload, count);
            dmx_state = DMX_DATA;
            return;

        case DMX_DATA:
            h264_decoder_feed(*mpeg2ts_user_data->h264_decoder, tspayload, count);
            return;
    }

#undef NEEDBYTES
}

static void mpeg2ts_parser_handle_tspacket(ymn::mpeg2ts_parser& parser, const uint8_t *tspacket)
{
    unsigned int adaptation_field_control;
    unsigned int payload_offset; // offset to the first byte in the payload or 0 if payload is not present
    unsigned int payload_unit_start_indicator;

    adaptation_field_control = (tspacket[3] >> 4) & 0x03;

    // 00 - reserved for future use by ISO/IEC
    // ITU-T Rec. H.222.0 | ISO/IEC 13818-1 decoders shall discard Transport Stream packets
    // with the adaptation_field_control field set to a value of '00'.
    // 01 - no adaptation_field, payload only
    // 10 - adaptation_field only, no payload
    // 11 - adaptation_field followed by payload
    switch (adaptation_field_control) {
        case  0: payload_offset = 0; break;
        case  1: payload_offset = 4; break;
        case  2: payload_offset = 0; break;
        case  3: payload_offset = 5 + tspacket[4]; break;
        default: payload_offset = 0; break;
    }

    if (payload_offset) {
        payload_unit_start_indicator = tspacket[1] & 0x40;
        mpeg2ts_parser_demux(
            parser,
            tspacket + payload_offset,
            MPEG2TS_PARSER_PACKET_SIZE - payload_offset,
            payload_unit_start_indicator);
    }
}

static void mpeg2ts_parser_parse(ymn::mpeg2ts_parser& parser)
{
    ymn::mpeg2ts_parser_status_e status;
    const uint8_t *tspacket;
    mpeg2ts_parser_user_data* mpeg2ts_user_data;
    ymn::mpeg2ts_pid received_pid;
    ymn::mpeg2ts_pid required_pid;

    mpeg2ts_user_data = static_cast<mpeg2ts_parser_user_data*>(parser.get_user_data());
    required_pid = mpeg2ts_user_data->pid;

    do {
        status = parser.parse();
        //std::cout << to_string(status) << std::endl;

        switch (status) {
            case ymn::mpeg2ts_parser_status_e::SYNC_GAINED:
                std::cout << to_string(status) << std::endl;
                break;

            case ymn::mpeg2ts_parser_status_e::SYNCHRONIZED:
                tspacket = parser.get_tspacket();
                received_pid = ymn::mpeg2ts_parser_get_pid(tspacket);
                if (received_pid == required_pid)
                    mpeg2ts_parser_handle_tspacket(parser, tspacket);
                break;

            case ymn::mpeg2ts_parser_status_e::SYNC_LOST:
                std::cout << to_string(status) << std::endl;
                break;

            case ymn::mpeg2ts_parser_status_e::NOT_SYNCHRONIZED:
                break;

            default:
                break;
       }
    } while (status != ymn::mpeg2ts_parser_status_e::NEED_BYTES);
}

static void mpeg2ts_parser_feed(ymn::mpeg2ts_parser& parser, const uint8_t* data, std::size_t count)
{
    std::size_t n_written;

    do {
        n_written = parser.write(data, count);
        if (0 == n_written) {
            parser.reset();
            continue;
        }

        if (n_written > count) {
            std::cerr << "error: parser swallowed more data then requested (this is really bad)" << std::endl;
            exit(EXIT_FAILURE);
        }

        mpeg2ts_parser_parse(parser);

        data += n_written;
        count -= n_written;
    } while (count > 0);
}

