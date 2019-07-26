/**
 * @file istream.hpp
 *
 * Definition of input stream class.
 *
 * It contains methods used to deserialize sequence of bytes and/or bits
 * into higher level data types.
 * Sequence of bytes can be interpreted as having little or big byte ordering
 * when deserializing to n-byte long integral data types.
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

#ifndef _ISTREAM_HPP_
#define _ISTREAM_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>
#include <functional> /* for std::function */

#include <cassert>

#if defined(DEBUG_ISTREAM)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "istream_position.hpp"
#include "endianness.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define STREAM_BYTE_ORDER_BIG_ENDIAN     0
#define STREAM_BYTE_ORDER_LITTLE_ENDIAN  1

#define IS_STREAM_AND_CPU_BIG_ENDIAN(STREAM_BYTE_ORDER) \
    ((STREAM_BYTE_ORDER == STREAM_BYTE_ORDER_BIG_ENDIAN) && \
     (get_cpu_endianness() == CPU_BIG_ENDIAN))

#define IS_STREAM_AND_CPU_LITTLE_ENDIAN(STREAM_BYTE_ORDER) \
    ((STREAM_BYTE_ORDER == STREAM_BYTE_ORDER_LITTLE_ENDIAN) && \
     (get_cpu_endianness() == CPU_LITTLE_ENDIAN))

#define HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER) \
    IS_STREAM_AND_CPU_BIG_ENDIAN(STREAM_BYTE_ORDER) || \
    IS_STREAM_AND_CPU_LITTLE_ENDIAN(STREAM_BYTE_ORDER)

/** no error was encountered during stream handling */
#define ISTREAM_STATUS_OK                    (0)
/** end of stream was reached */
#define ISTREAM_STATUS_EOS_REACHED           (1 << 0)
/** an attempt to read/peak from not byte alligned stream was spotted */
#define ISTREAM_STATUS_IMPROPER_ALLIGMENT    (1 << 1)
/** stream was marked as corrupted by user */
#define ISTREAM_STATUS_STREAM_CORRUPTED      (1 << 2)

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

static inline const char* istream_status_to_string(uint32_t status)
{
    if (status == ISTREAM_STATUS_OK)
        return "ok";
    else {
        std::ostringstream stream;

        if (status & ISTREAM_STATUS_EOS_REACHED) {
            stream << "eos";
            if (status > ISTREAM_STATUS_EOS_REACHED)
                stream << ", ";
        }

        if (status & ISTREAM_STATUS_IMPROPER_ALLIGMENT) {
            stream << "alligment";
            if (status > ISTREAM_STATUS_IMPROPER_ALLIGMENT)
                stream << ", ";
        }

        if (status & ISTREAM_STATUS_STREAM_CORRUPTED) {
            stream << "corrupted";
            if (status > ISTREAM_STATUS_STREAM_CORRUPTED)
                stream << ", ";
        }

        return stream.str().c_str();
    }
}

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

enum class istream_seek_e : int32_t
{
    /** current position */
    CURRENT,
    /** beginning of a stream */
    BEGINNING,
    /** end of a stream */
    END,
};

typedef std::function<void(const istream_position& before, const istream_position& after)> istream_error_function;

template<int STREAM_BYTE_ORDER = STREAM_BYTE_ORDER_BIG_ENDIAN>
class istream
{
    static_assert(get_cpu_endianness() == CPU_BIG_ENDIAN ||
                  get_cpu_endianness() == CPU_LITTLE_ENDIAN,
                  "unknown cpu endiannes");
public:
    explicit istream(const uint8_t* buffer = nullptr, std::size_t size = 0, const istream_error_function& function = nullptr) :
        m_buffer(buffer),
        m_size(size),
        m_position(),
        m_status(ISTREAM_STATUS_OK),
        m_error_function(function)
    {
#if defined(DEBUG_ISTREAM)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    ~istream()
    {
#if defined(DEBUG_ISTREAM)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    istream(const istream& other) :
        m_buffer(other.m_buffer),
        m_size(other.m_size),
        m_position(other.m_position),
        m_status(other.m_status),
        m_error_function(other.m_error_function)
    {
#if defined(DEBUG_ISTREAM)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    istream& operator = (const istream& other)
    {
        m_buffer         = other.m_buffer;
        m_size           = other.m_size;
        m_position       = other.m_position;
        m_status         = other.m_status;
        m_error_function = other.m_error_function;

        return *this;
    }

    bool is_valid() const
    {
        return m_buffer != nullptr;
    }

    uint32_t status() const
    {
        return m_status;
    }

    void mark_corrupted()
    {
       m_status |= ISTREAM_STATUS_STREAM_CORRUPTED;
    }

    void clear_status()
    {
        m_status = ISTREAM_STATUS_OK;
    }

    void set_error_function(const istream_error_function& function)
    {
        m_error_function = function;
    }

    void clear_error_function()
    {
        m_error_function = nullptr;
    }

    std::size_t size() const
    {
        return m_size;
    }

    const uint8_t* data() const
    {
        return m_buffer;
    }

    const uint8_t* current_data_pointer() const
    {
        return m_buffer + m_position.m_byte_position;
    }

    bool read_u8(uint8_t& value)
    {
        return read(value);
    }

    bool peek_u8(uint8_t& value)
    {
        return peek(value);
    }

    bool read_s8(int8_t& value)
    {
        return read(value);
    }

    bool peek_s8(int8_t& value)
    {
       return peek(value);
    }

    bool read_u16(uint16_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return read(value);
        else {
            uint16_t v;
            bool status = read(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool peek_u16(uint16_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return peek(value);
        else {
            uint16_t v;
            bool status = peek(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool read_s16(int16_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return read(value);
        else {
            uint16_t v;
            bool status = read(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool peek_s16(int16_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return peek(value);
        else {
            uint16_t v;
            bool status = peek(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool read_u32(uint32_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return read(value);
        else {
            uint32_t v;
            bool status = read(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool peek_u32(uint32_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return peek(value);
        else {
            uint32_t v;
            bool status = peek(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool read_s32(int32_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return read(value);
        else {
            uint32_t v;
            bool status = read(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool peek_s32(int32_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return peek(value);
        else {
            uint32_t v;
            bool status = peek(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool read_u64(uint64_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return read(value);
        else {
            uint64_t v;
            bool status = read(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool peek_u64(uint64_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return peek(value);
        else {
            uint64_t v;
            bool status = peek(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool read_s64(int64_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return read(value);
        else {
            uint64_t v;
            bool status = read(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    bool peek_s64(int64_t& value)
    {
        if (HAS_STREAM_AND_CPU_THE_SAME_BYTE_ORDER(STREAM_BYTE_ORDER))
            return peek(value);
        else {
            uint64_t v;
            bool status = peek(v);
            if (status)
                value = swap(v);
            return status;
        }
    }

    /**
     * Reads subsequent 'number_of_bits' bits from the current stream position
     * and interprets them as uint32_t value.
     *
     * If the read operation would cause that stream position is moved beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[in] number_of_bits Number of bits to be read.
     * @param[out] value 'uint32_t' representation of retrieved bit pattern.
     *
     * @return true on success, false on failure.
     */
    bool read_bits(uint32_t number_of_bits, uint32_t& value)
    {
        assert(number_of_bits <= 32);

        /* verify whether we don't go beyond size of the stream */
        if ((m_position.to_bits() + number_of_bits) <= (m_size * BITS_PER_BYTE)) {

            uint32_t v = 0;
            uint32_t bits_remaining = number_of_bits;
            uint32_t bits_to_read;

            while (bits_remaining > 0) {
                /* there is more bits to be read than bits left in current byte */
                if ((m_position.m_bit_position + bits_remaining) >= BITS_PER_BYTE ) {
                    /* number of bits left in current byte */
                    bits_to_read = BITS_PER_BYTE - m_position.m_bit_position;

                    /* shift the result by number of bits to be read
                    to make room for new data */
                    v <<= bits_to_read;
                    /* add to the result value */
                    v |= m_buffer[m_position.m_byte_position] & ((1U << bits_to_read) - 1);

                    /* move to the next byte in the stream */
                    m_position.m_byte_position++;
                    /* and reset bit position to the begining of a byte */
                    m_position.m_bit_position = 0;
                }
                else {
                    /* there remains less than 8 bits to be read */
                    bits_to_read = bits_remaining;

                    /* shift the result by number of bits to be read
                    to make room for new data */
                    v <<= bits_to_read;

                    /* advance bit position */
                    m_position.m_bit_position += bits_to_read;

                    /* add to the result value */
                    v |= (m_buffer[m_position.m_byte_position] >> (BITS_PER_BYTE - m_position.m_bit_position)) & ((1U << bits_to_read) - 1);
                }

                /* decrease number of remaining bits by number of bits read in this iteration */
                bits_remaining -= bits_to_read;
          }

          value = v;
          return true;
       }
       else {
            m_status |= ISTREAM_STATUS_EOS_REACHED;
            if (m_error_function)
                m_error_function(m_position, m_position + number_of_bits);
            return false;
       }
    }

    /**
     * Peeks subsequent 'number_of_bits' bits from the current stream position
     * and interprets them as uint32_t value.
     *
     * If the peek operation would cause that stream position is touched beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[in] number_of_bits Number of bits to be peek.
     * @param[out] value 'uint32_t' representation of retrieved bit pattern.
     *
     * @return true on success, false on failure.
     */
    bool peek_bits(uint32_t number_of_bits, uint32_t& value)
    {
        assert(number_of_bits <= 32);

        /* verify whether we don't go beyond size of the stream */
        if ((m_position.to_bits() + number_of_bits) <= (m_size * BITS_PER_BYTE)) {

            uint32_t v = 0;
            uint32_t bits_remaining = number_of_bits;
            uint32_t bits_to_read;

            uint32_t byte_pos = m_position.m_byte_position;
            uint32_t bit_pos = m_position.m_bit_position;

            while (bits_remaining > 0) {
                /* there is more bits to be peeked than bits left in current byte */
                if ((bit_pos + bits_remaining) >= BITS_PER_BYTE) {
                    /* number of bits left in current byte */
                    bits_to_read = BITS_PER_BYTE - bit_pos;

                    /* shift the result by number of bits to be peeked
                    to make room for new data */
                    v <<= bits_to_read;
                    /* add to the result value */
                    v |= m_buffer[byte_pos] & ((1U << bits_to_read) - 1);

                    /* move to the next byte in the stream */
                    byte_pos++;
                    /* and reset bit position to the begining of a byte */
                    bit_pos = 0;
                }
                else {
                    /* there remains less than 8 bits to be peeked */
                    bits_to_read = bits_remaining;

                    /* shift the result by number of bits to be peeked
                    to make room for new data */
                    v <<= bits_to_read;

                    /* advance bit position */
                    bit_pos += bits_to_read;

                    /* add to the result value */
                    v |= (m_buffer[byte_pos] >> (BITS_PER_BYTE - bit_pos)) & ((1U << bits_to_read) - 1);
                }

                /* decrease the number of remaining bits by number of bits peeked in this iteration */
                bits_remaining -= bits_to_read;
            }

            value = v;
            return true;
        }
        else {
            m_status |= ISTREAM_STATUS_EOS_REACHED;
            if (m_error_function)
                m_error_function(m_position, m_position + number_of_bits);
            return false;
        }
    }

    /**
     * Reads unsigned Exp-Golomb codes.
     *
     * Parsing process for unsigned Exp-Golomb codes begins with reading the bits
     * starting at the current location in the bitstream up to and including
     * the first non-zero bit, and counting the number of leading bits that
     * are equal to 0. The value of the Exp-Golomb code is then
     *
     * code = (1U << leading_zero_bits) - 1 + stream_read_bits(stream, leading_zero_bits)
     *
     * If the read operation would cause that stream position is moved beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[out] value 'uint32_t' representation of retrieved bit pattern.
     *
     * @return true on success, false on failure.
     */
    bool read_exp_golomb_u(uint32_t& value)
    {
        bool status;
        uint32_t leading_zero_bits;
        uint32_t v;

        leading_zero_bits = 0;
        while ((status = read_bits(1, v)) && (0 == v))
            leading_zero_bits++;

        if (!status) {
            m_position -= leading_zero_bits;
            return false;
        }

        if (leading_zero_bits) {
            status = read_bits(leading_zero_bits, v);
            if (!status) {
                m_position -= leading_zero_bits + 1;
                return false;
            }
            v += (1U << leading_zero_bits) - 1;
        } else
            v = 0;

        value = v;
        return true;
    }

    /**
     * Peeks unsigned Exp-Golomb codes.
     *
     * Parsing process for unsigned Exp-Golomb codes begins with peeking the bits
     * starting at the current location in the bitstream up to and including
     * the first non-zero bit, and counting the number of leading bits that
     * are equal to 0. The value of the Exp-Golomb code is then
     *
     * code = (1U << leading_zero_bits) - 1 + stream_read_bits(stream, leading_zero_bits)
     *
     * If the peek operation would cause that stream position is touched beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[out] value 'uint32_t' representation of retrieved bit pattern.
     *
     * @return true on success, false on failure.
     */
    bool peek_exp_golomb_u(uint32_t& value)
    {
        bool status;
        uint32_t leading_zero_bits;
        uint32_t v;

        leading_zero_bits = 0;
        while ((status = read_bits(1, v)) && (0 == v))
            leading_zero_bits++;

        if (!status) {
            m_position -= leading_zero_bits;
            return false;
        }

        if (leading_zero_bits) {
            status = read_bits(leading_zero_bits, v);
            if (!status) {
                m_position -= leading_zero_bits + 1;
                return false;
            }
            v += (1U << leading_zero_bits) - 1;
        } else
            v = 0;

        value = v;
        m_position -= 2 * leading_zero_bits + 1;
        return true;
    }

    /**
     * Reads signed Exp-Golomb codes.
     *
     * Parsing process for signed Exp-Golomb codes is a two stage process.
     * First stage is identical to reading an unsigned Exp-Golomb code.
     * Output of the first stage (let's say k) is then given to the following
     * formula (which is a second stage of reading signed Exp-Golomb codes):
     *
     * code = (-1)^(k + 1) * ((k + 1) / 2)
     *
     * If the read operation would cause that stream position is moved beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[out] value 'int32_t' representation of retrieved bit pattern.
     *
     * @return true on success, false on failure.
     */
    bool read_exp_golomb_s(int32_t& value)
    {
        bool status;
        uint32_t leading_zero_bits;
        uint32_t v;

        leading_zero_bits = 0;
        while ((status = read_bits(1, v)) && (0 == v))
            leading_zero_bits++;

        if (!status) {
            m_position -= leading_zero_bits;
            return false;
        }

        if (leading_zero_bits) {
            status = read_bits(leading_zero_bits, v);
            if (!status) {
                m_position -= leading_zero_bits + 1;
                return false;
            }

            v += (1U << leading_zero_bits) - 1;
            if (v & 0x01)
                v = (v + 1) / 2;
            else
                v = -(v / 2);
        } else
            v = 0;

        value = v;
        return true;
    }

    /**
     * Peeks signed Exp-Golomb codes.
     *
     * Parsing process for signed Exp-Golomb codes is a two stage process.
     * First stage is identical to peeking an unsigned Exp-Golomb code.
     * Output of the first stage (let's say k) is then given to the following
     * formula (which is a second stage of peeking signed Exp-Golomb codes):
     *
     * code = (-1)^(k + 1) * ((k + 1) / 2)
     *
     * If the peek operation would cause that stream position is touched beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[out] value 'int32_t' representation of retrieved bit pattern.
     *
     * @return true on success, false on failure.
     */
    bool peek_exp_golomb_s(int32_t& value)
    {
        bool status;
        uint32_t leading_zero_bits;
        uint32_t v;

        leading_zero_bits = 0;
        while ((status = read_bits(1, v)) && (0 == v))
            leading_zero_bits++;

        if (!status) {
            m_position -= leading_zero_bits;
            return false;
        }

        if (leading_zero_bits) {
            status = read_bits(leading_zero_bits, v);
            if (!status) {
                m_position -= leading_zero_bits + 1;
                return false;
            }

            v += (1U << leading_zero_bits) - 1;
            if (v & 0x01)
                v = (v + 1) / 2;
            else
                v = -(v / 2);
        } else
            v = 0;

        value = v;
        m_position -= 2 * leading_zero_bits + 1;
        return true;
    }

    /**
     * Skips current position in the stream by a specified number of bytes.
     *
     * You can use 'skip()' method to skip any number of bytes, even beyond
     * the end of the stream. No error will be reported in such a situation,
     * but when the stream position is beyond the stream boundaries,
     * then subsequent read and peek operations will fail.
     *
     * @param[in] number_of_bytes Number of bytes to skip.
     *
     * @return none
     */
    void skip(std::size_t number_of_bytes)
    {
        m_position.m_byte_position += number_of_bytes;
    }

    /**
     * Skips current position in the stream by a specified number of bits.
     *
     * You can use 'skip_bits()' method to skip given number of bits, even beyond
     * the end of the stream. No error will be reported in such a situation,
     * but when the stream position is beyond the stream boundaries,
     * then subsequent read and peek operations will fail.
     *
     * @param[in] number_of_bits Number of bits to skip.
     *
     * @return none
     */
    void skip_bits(std::size_t number_of_bits)
    {
        m_position += number_of_bits;
    }

    /**
     * Gets the current position of a stream.
     *
     * The position is expressed as an offset (given in bytes)
     * relative to the beginning of the stream.
     *
     * @return Current position of a stream.
     */
    std::size_t tell() const
    {
        return m_position.m_byte_position;
    }

    /**
     * Gets the current bit position of a stream.
     *
     * The bit position is expressed as an offset (given in bits)
     * within a byte where subsequent read/peek operation will start from.
     * Thus the bit position reaches the values from
     * 0 (most significant bit) till 7 (least significant bit).
     *
     * @return Current bit position of a stream.
     */
    std::size_t tell_bits() const
    {
        return m_position.m_bit_position;
    }

    /**
     * Moves the stream position to a specified location.
     *
     * The location is calculated by adding an offset (given in bytes)
     * to the 'origin'. The next operation on the stream will take place
     * at the new location.
     * You can use 'seek()' method to reposition the position anywhere in a stream.
     * The position can also be positioned beyond the end of the stream.
     * No error will be reported by this method, but when the stream position
     * is beyond the stream boundaries, then subsequent read and peak operations will fail.
     *
     * @param[in] offset Specifies number of bytes from origin.
     * @param[in] origin Initial position.
     */
    void seek(long offset, istream_seek_e origin)
    {
        switch (origin) {
            case istream_seek_e::CURRENT:
                m_position.m_byte_position += offset;
                break;

            case istream_seek_e::BEGINNING:
                m_position.m_byte_position = offset;
                break;

            case istream_seek_e::END:
                m_position.m_byte_position = m_size + offset;
                break;

            default:
                break;
        }
    }

    /**
     * Gets number of bytes which remain unread in the stream.
     *
     * @return Number of bytes which remain unread in the stream.
     */
    long remains() const
    {
        return m_size - m_position.m_byte_position;
    }

    /**
     * Sets the position in the stream at the beginning.
     *
     * @return none
     */
    void rewind()
    {
        m_position.reset();
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << "istream@";
        stream << std::hex << this;
        stream << " [";
        stream << "size: ";
        stream << std::dec << m_size;
        stream << ", position: ";
        stream << static_cast<std::string>(m_position);
        stream << ", status: ";
        stream << istream_status_to_string(m_status);
        stream << "]";

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

private:
    template<typename T>
    bool read(T& value)
    {
        if (m_position.m_bit_position) {
            m_status |= ISTREAM_STATUS_IMPROPER_ALLIGMENT;
            if (m_error_function)
                m_error_function(m_position, m_position + sizeof(T) * BITS_PER_BYTE);
            return false;
        }

        const void* p = readbytes(sizeof(T));
        if (p) {
            value = *(reinterpret_cast<const T*>(p));
            return true;
        }
        else {
            m_status |= ISTREAM_STATUS_EOS_REACHED;
            if (m_error_function)
                m_error_function(m_position, m_position + sizeof(T) * BITS_PER_BYTE);
            return false;
        }
    }

    template<typename T>
    bool peek(T& value)
    {
        if (m_position.m_bit_position) {
            m_status |= ISTREAM_STATUS_IMPROPER_ALLIGMENT;
            if (m_error_function)
                m_error_function(m_position, m_position + sizeof(T) * BITS_PER_BYTE);
            return false;
        }

        const void* p = peekbytes(sizeof(T));
        if (p) {
            value = *(reinterpret_cast<const T*>(p));
            return true;
        }
        else {
            m_status |= ISTREAM_STATUS_EOS_REACHED;
            if (m_error_function)
                m_error_function(m_position, m_position + sizeof(T) * BITS_PER_BYTE);
            return false;
        }
    }

    bool align(const std::size_t size)
    {
        const std::size_t mask = size - 1;
        const std::size_t bytepos = (m_position.m_byte_position + mask) & (~mask);

        if (m_position.m_bit_position) {
            m_status |= ISTREAM_STATUS_IMPROPER_ALLIGMENT;
            if (m_error_function)
                m_error_function(m_position, m_position + bytepos * BITS_PER_BYTE);
            return false;
        }

        if (bytepos <= m_size) {
            m_position.m_byte_position = bytepos;
            return true;
        }
        else {
            m_status |= ISTREAM_STATUS_EOS_REACHED;
            if (m_error_function)
                m_error_function(m_position, m_position + bytepos * BITS_PER_BYTE);
            return false;
        }
    }

    const void* readbytes(const std::size_t size)
    {
        const void* p;
        const std::size_t pos = m_position.m_byte_position + size;

        if (pos <= m_size) {
            p = m_buffer + m_position.m_byte_position;
            m_position.m_byte_position = pos;
        }
        else {
            p = nullptr;
        }

        return p;
    }

    const void* peekbytes(const std::size_t size) const
    {
        const void* p;
        const std::size_t pos = m_position.m_byte_position + size;

        if (pos <= m_size) {
            p = m_buffer + m_position.m_byte_position;
        }
        else {
            p = nullptr;
        }

        return p;
    }

    /** A pointer to the memory buffer constituing a stream data. */
    const uint8_t* m_buffer;

    /** Only bytes whitin the range m_buffer[0] ... m_buffer[m_size - 1]
    are valid stream data. */
    std::size_t m_size;

    /** Current stream position. */
    istream_position m_position;

    /** Stream's status */
    uint32_t m_status;

    /** Stores error function. Can be nullptr in which case
    error function is considered as not registered. */
    istream_error_function m_error_function;
};

using istream_be = istream<STREAM_BYTE_ORDER_BIG_ENDIAN>;
using istream_le = istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN>;

} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

#endif /* _ISTREAM_HPP_ */
