/**
 * @file endianness.hpp
 *
 * This header file defines set of functions responsible for converting
 * byte encoding scheme of integer values (uint16_t, uint32_t, uint64_t).
 * Such conversions can be performed from byte order applicable on
 * current CPU (host) to the little-endian or big-endian byte order.
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

#ifndef _ENDIANNESS_HPP_
#define _ENDIANNESS_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define CPU_ENDIANESS_NOT_DETECTED  0
#define CPU_BIG_ENDIAN              1
#define CPU_LITTLE_ENDIAN           2
#define CPU_UNKNOWN_ENDIAN          3

#if defined(__BYTE_ORDER)
# if __BYTE_ORDER == __BIG_ENDIAN
#  define CPU_ENDIANNESS CPU_BIG_ENDIAN
# elif __BYTE_ORDER == __LITTLE_ENDIAN
#  define CPU_ENDIANNESS CPU_LITTLE_ENDIAN
# endif
#endif

#if defined(CPU_ENDIANNESS) && \
    (CPU_ENDIANNESS == CPU_BIG_ENDIAN || CPU_ENDIANNESS == CPU_LITTLE_ENDIAN)
# define CONSTEXPR constexpr
#else
# define CONSTEXPR
#endif

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

static inline CONSTEXPR int get_cpu_endianness()
{
#if CPU_ENDIANNESS == CPU_BIG_ENDIAN
    return CPU_BIG_ENDIAN;
#elif CPU_ENDIANNESS == CPU_LITTLE_ENDIAN
    return CPU_LITTLE_ENDIAN;
#else
    static int endianness = CPU_ENDIANESS_NOT_DETECTED;

    if (endianness == CPU_ENDIANESS_NOT_DETECTED) {
        union {
            int i;
            uint8_t c[sizeof(int)];
        } x = {0x12345678};

        if (x.c[0] == 0x12)
            endianness = CPU_BIG_ENDIAN;
        else
        if (x.c[0] == 0x78)
            endianness = CPU_LITTLE_ENDIAN;
        else
            endianness = CPU_UNKNOWN_ENDIAN;
    }

    return endianness;
#endif
}

static inline constexpr uint16_t swap(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

static inline constexpr uint32_t swap(uint32_t x)
{
    x = ((x << 8) & 0xFF00FF00U) | ((x >> 8) & 0x00FF00FFU);
    return (x << 16) | (x >> 16);
}

static inline constexpr uint64_t swap(uint64_t x)
{
    x = ((x <<  8) & 0xFF00FF00FF00FF00ULL) | ((x >>  8) & 0x00FF00FF00FF00FFULL);
    x = ((x << 16) & 0xFFFF0000FFFF0000ULL) | ((x >> 16) & 0x0000FFFF0000FFFFULL);
    return (x << 32) | (x >> 32);
}

static inline CONSTEXPR uint16_t cputobe16(uint16_t cpu16)
{
    if (CPU_BIG_ENDIAN == get_cpu_endianness())
        return cpu16;
    else
        return swap(cpu16);
}

static inline CONSTEXPR uint16_t cputole16(uint16_t cpu16)
{
    if (CPU_LITTLE_ENDIAN == get_cpu_endianness())
        return cpu16;
    else
        return swap(cpu16);
}

static inline CONSTEXPR uint16_t be16tocpu(uint16_t be16)
{
    if (CPU_BIG_ENDIAN == get_cpu_endianness())
        return be16;
    else
        return swap(be16);
}

static inline CONSTEXPR uint16_t le16tocpu(uint16_t le16)
{
    if (CPU_LITTLE_ENDIAN == get_cpu_endianness())
        return le16;
    else
        return swap(le16);
}

static inline CONSTEXPR uint32_t cputobe32(uint32_t cpu32)
{
    if (CPU_BIG_ENDIAN == get_cpu_endianness())
        return cpu32;
    else
        return swap(cpu32);
}

static inline CONSTEXPR uint32_t cputole32(uint32_t cpu32)
{
    if (CPU_LITTLE_ENDIAN == get_cpu_endianness())
        return cpu32;
    else
        return swap(cpu32);
}

static inline CONSTEXPR uint32_t be32tocpu(uint32_t be32)
{
    if (CPU_BIG_ENDIAN == get_cpu_endianness())
        return be32;
    else
        return swap(be32);
}

static inline CONSTEXPR uint32_t le32tocpu(uint32_t le32)
{
    if (CPU_LITTLE_ENDIAN == get_cpu_endianness())
        return le32;
    else
        return swap(le32);
}

static inline CONSTEXPR uint64_t cputobe64(uint64_t cpu64)
{
    if (CPU_BIG_ENDIAN == get_cpu_endianness())
        return cpu64;
    else
        return swap(cpu64);
}

static inline CONSTEXPR uint64_t cputole64(uint64_t cpu64)
{
    if (CPU_LITTLE_ENDIAN == get_cpu_endianness())
        return cpu64;
    else
        return swap(cpu64);
}

static inline CONSTEXPR uint64_t be64tocpu(uint64_t be64)
{
    if (CPU_BIG_ENDIAN == get_cpu_endianness())
        return be64;
    else
        return swap(be64);
}

static inline CONSTEXPR uint64_t le64tocpu(uint64_t le64)
{
    if (CPU_LITTLE_ENDIAN == get_cpu_endianness())
        return le64;
    else
        return swap(le64);
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

} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

#endif /* _ENDIANNESS_HPP_ */
