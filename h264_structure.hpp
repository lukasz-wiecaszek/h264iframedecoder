/**
 * @file h264_structure.hpp
 *
 * Base class for ISO/IEC 14496-10/ITU-T H.264 syntax structures.
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

#ifndef _H264_STRUCTURE_HPP_
#define _H264_STRUCTURE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "istream.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

inline bool more_rbsp_data(istream_be& s)
{
    std::size_t bits;
    uint32_t value;

    if (s.tell() >= s.size())
        return false;

    if (s.tell() < s.size() - 1)
        return true;

    bits = 8 - s.tell_bits();

    if (!s.peek_bits(bits, value))
        return false;

    if (value == (1U << (bits - 1)))
        return false;

    return true;
}

} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

class h264_structure
{
public:
    explicit h264_structure() :
        m_valid(false)
    {
    }

    bool is_valid() const
    {
        return m_valid;
    }

    void set_valid(bool valid)
    {
        m_valid = valid;
    }

    void reset()
    {
        m_valid = false;
    }

    virtual std::string to_string() const = 0;

    operator std::string () const
    {
        return to_string();
    }

protected:
    ~h264_structure() = default;

private:
    bool m_valid;
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

#endif /* _H264_STRUCTURE_HPP_ */
