/**
 * @file mpeg2ts_parser.hpp
 *
 * Definition of MPEG2 Transport Stream (ISO/IEC 13818-1) parser.
 *
 * MPEG2 Transport Stream syntax and semantics is defined in ISO/IEC 13818-1
 * "Information technology — Generic coding of moving pictures and associated
 * audio information: Systems".
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

#ifndef _MPEG2TS_PARSER_HPP_
#define _MPEG2TS_PARSER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>
#include <functional>

#include <cstring>
#include <climits>

#if defined(DEBUG_PARSERS)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "mpeg2ts_pid.hpp"
#include "base_parser.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define MPEG2TS_PARSER_PACKET_SIZE       188

#define MPEG2TS_PARSER_STATUSES \
    MPEG2TS_PARSER_STATUS(NEED_BYTES) \
    MPEG2TS_PARSER_STATUS(SYNC_GAINED) \
    MPEG2TS_PARSER_STATUS(SYNCHRONIZED) \
    MPEG2TS_PARSER_STATUS(SYNC_LOST) \
    MPEG2TS_PARSER_STATUS(NOT_SYNCHRONIZED) \
    MPEG2TS_PARSER_STATUS(TRANSPORT_RATE_DETECTED) \

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

static inline mpeg2ts_pid mpeg2ts_parser_get_pid(const uint8_t* tsp)
{
   return mpeg2ts_pid(((tsp[1] << 8) + tsp[2]) & 0x1fff);
}

static inline bool mpeg2ts_parser_has_pcr(const uint8_t* tsp)
{
   return ((tsp[3] & 0x20)  &&  // if adaptation field is present
           (tsp[4])         &&  // and has non-zero value
           (tsp[5] & 0x10));    // and if PCR flag is present
}

static inline uint64_t mpeg2ts_parser_get_pcr(const uint8_t* tsp)
{
   uint64_t pcr_base = (((uint64_t)((tsp[6] << 24) + (tsp[7] << 16) + (tsp[8] << 8) + tsp[9]) << 8) | tsp[10]) >> 7;
   return 1ULL + pcr_base * 300 + (((tsp[10] << 8) | tsp[11]) & 0x1ff);
}

} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

enum class mpeg2ts_parser_status_e : int32_t
{
#define MPEG2TS_PARSER_STATUS(id) id,
    MPEG2TS_PARSER_STATUSES
#undef MPEG2TS_PARSER_STATUS
};

constexpr static inline const char* to_string(mpeg2ts_parser_status_e e)
{
    const char *str = "invalid 'mpeg2ts_parser_status_e' value";

    switch (e) {
#define MPEG2TS_PARSER_STATUS(id) case mpeg2ts_parser_status_e::id: str = #id; break;
            MPEG2TS_PARSER_STATUSES
#undef MPEG2TS_PARSER_STATUS
    }

    return str;
}

class mpeg2ts_parser : public base_parser<uint8_t>
{
public:
    explicit mpeg2ts_parser(std::size_t capacity) :
        base_parser<uint8_t>(capacity),
        m_state_function(&mpeg2ts_parser::waiting_for_sync),
        m_tspacket_size(0),
        m_transport_rate()
    {
#if defined(DEBUG_PARSERS)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << to_string() << std::endl;
#endif
    }

    virtual ~mpeg2ts_parser()
    {
#if defined(DEBUG_PARSERS)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    mpeg2ts_parser(const mpeg2ts_parser&) = delete;
    mpeg2ts_parser(mpeg2ts_parser&&) = delete;
    mpeg2ts_parser& operator = (const mpeg2ts_parser&) = delete;
    mpeg2ts_parser& operator = (mpeg2ts_parser&&) = delete;

    void reset()
    {
        base_parser::reset();
        m_state_function = &mpeg2ts_parser::waiting_for_sync;
        m_tspacket_size = 0;
        m_transport_rate.reset();
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << base_parser::to_string();
        stream << std::endl;
        stream << "mpeg2ts_parser@";
        stream << std::hex << this;
        stream << " [m_tspacket_size: ";
        stream << std::dec << m_tspacket_size;
        stream << ", ";
        stream << "m_tspacket_rate: ";
        stream << std::dec << m_transport_rate;
        stream << "]";

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    mpeg2ts_parser_status_e parse()
    {
        return (this->*m_state_function)();
    }

    const uint8_t* get_tspacket()
    {
        const uint8_t *p;

        p = m_flatbuffer.read_ptr();
        m_flatbuffer.consume(m_tspacket_size);

        return p;
    }

    std::size_t get_tspacket_size() const
    {
        return m_tspacket_size;
    }

    std::size_t get_tspacket_rate() const
    {
        return m_transport_rate;
    }

private:
    int synchronize(const std::size_t tspacket_size);

    mpeg2ts_parser_status_e waiting_for_sync();
    mpeg2ts_parser_status_e synchronized();

    mpeg2ts_parser_status_e (mpeg2ts_parser::*m_state_function)();
    std::size_t m_tspacket_size;

    struct transport_rate
    {
        explicit transport_rate() :
            m_pcr_pid(MPEG2TS_PID_INVALID),
            m_pcr1(0),
            m_pcr2(0),
            m_no_of_packets(0),
            m_transport_rate(0)
        {
        }

        bool is_valid() const
        {
            return m_transport_rate != 0;
        }

        void reset()
        {
            m_pcr_pid = MPEG2TS_PID_INVALID;
            m_pcr1 = 0;
            m_pcr2 = 0;
            m_no_of_packets = 0;
            m_transport_rate = 0;
        }

        operator std::size_t () const
        {
            return m_transport_rate;
        }

        bool calculate(const uint8_t* tsp);

    private:
        mpeg2ts_pid m_pcr_pid;
        uint64_t m_pcr1;
        uint64_t m_pcr2;
        uint64_t m_no_of_packets;
        std::size_t m_transport_rate; /* ts packets per second */
    };

    transport_rate m_transport_rate;
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

#endif /* _MPEG2TS_PARSER_HPP_ */
