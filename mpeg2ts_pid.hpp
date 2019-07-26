/**
 * @file mpeg2ts_pid.hpp
 *
 * Definition of MPEG2 Transport Stream (ISO/IEC 13818-1) Packet ID type.
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

#ifndef _MPEG2TS_PID_HPP_
#define _MPEG2TS_PID_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "ioutilities.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define MPEG2TS_PID_MIN        0x0000
#define MPEG2TS_PID_MAX        0x1fff
#define MPEG2TS_PID_INVALID    0x2000

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

class mpeg2ts_pid
{
public:
    mpeg2ts_pid(int pid = MPEG2TS_PID_INVALID) :
        m_pid(pid)
    {
    }

    bool is_valid() const
    {
        return (m_pid >= MPEG2TS_PID_MIN) && (m_pid <= MPEG2TS_PID_MAX);
    }

    operator int () const
    {
        return m_pid;
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << HEXDEC(m_pid);

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

private:
    int m_pid;
};

} /* end of namespace ymn */

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

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

#endif /* _MPEG2TS_PID_HPP_ */
