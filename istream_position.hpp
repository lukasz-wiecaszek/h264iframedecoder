/**
 * @file istream_position.hpp
 *
 * Helper class used to store current stream position.
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

#ifndef _ISTREAM_POSITION_HPP_
#define _ISTREAM_POSITION_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

#if defined(DEBUG_ISTREAM)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define BITS_PER_BYTE 8

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

} /* end of namespace ymn */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace ymn
{

struct istream_position
{
    constexpr explicit istream_position(std::size_t byte_position = 0, std::size_t bit_position = 0) :
        m_byte_position(byte_position),
        m_bit_position(bit_position)
    {
    }

    constexpr istream_position& operator = (const istream_position& other)
    {
        m_byte_position  = other.m_byte_position;
        m_bit_position   = other.m_bit_position;

        return *this;
    }

    constexpr bool operator == (const istream_position& other) const
    {
        return ((m_byte_position == other.m_byte_position) &&
                (m_bit_position  == other.m_bit_position));
    }

    constexpr bool operator != (const istream_position& other) const
    {
        return !(*this == other);
    }

    constexpr istream_position operator + (const istream_position& other) const
    {
        std::size_t number_of_bits = to_bits() + other.to_bits();

        istream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

        return pos;
    }

    constexpr istream_position operator - (const istream_position& other) const
    {
        std::size_t number_of_bits = to_bits() - other.to_bits();

        istream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

        return pos;
    }

    constexpr istream_position operator += (const istream_position& other)
    {
        std::size_t number_of_bits = to_bits() + other.to_bits();

        m_byte_position = number_of_bits / BITS_PER_BYTE;
        m_bit_position = number_of_bits % BITS_PER_BYTE;

        return *this;
    }

    constexpr istream_position operator -= (const istream_position& other)
    {
        std::size_t number_of_bits = to_bits() - other.to_bits();

        m_byte_position = number_of_bits / BITS_PER_BYTE;
        m_bit_position = number_of_bits % BITS_PER_BYTE;

        return *this;
    }

    constexpr istream_position operator += (std::size_t bits)
    {
        std::size_t number_of_bits = to_bits() + bits;

        m_byte_position = number_of_bits / BITS_PER_BYTE;
        m_bit_position = number_of_bits % BITS_PER_BYTE;

        return *this;
    }

    constexpr istream_position operator -= (std::size_t bits)
    {
        std::size_t number_of_bits = to_bits() - bits;

        m_byte_position = number_of_bits / BITS_PER_BYTE;
        m_bit_position = number_of_bits % BITS_PER_BYTE;

        return *this;
    }

    constexpr std::size_t to_bits() const
    {
        return m_byte_position * BITS_PER_BYTE + m_bit_position;
    }

    constexpr void reset()
    {
        m_byte_position = 0;
        m_bit_position = 0;
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << m_byte_position;
        stream << ":";
        stream << m_bit_position;

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    /**
     * Stores position of the next byte to be read from the stream's buffer
     * (relative to the beginning of the buffer).
     */
    std::size_t m_byte_position;

    /**
     * Stores position of the next bit to be read from the stream's buffer
     * (relative to the current byte position).
     * Bit position is expressed as the offset from byte boundary,
     * most significiant bit first.
     * Thus bit position takes values from 0 (msb) till 7 (lsb).
     */
    std::size_t m_bit_position;
};

constexpr inline istream_position operator + (const istream_position& other, std::size_t bits)
{
    std::size_t number_of_bits = other.to_bits() + bits;

    istream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

    return pos;
}

constexpr inline istream_position operator + (std::size_t bits, const istream_position& other)
{
    std::size_t number_of_bits = bits + other.to_bits();

    istream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

    return pos;
}

constexpr inline istream_position operator - (const istream_position& other, std::size_t bits)
{
    std::size_t number_of_bits = other.to_bits() + bits;

    istream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

    return pos;
}

constexpr inline istream_position operator - (std::size_t bits, const istream_position& other)
{
    std::size_t number_of_bits = bits + other.to_bits();

    istream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

    return pos;
}

} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

#endif /* _ISTREAM_POSITION_HPP_ */
