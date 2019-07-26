/**
 * @file scaling_matrices.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 scaling matrices.
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

#ifndef _SCALING_MATRICES_HPP_
#define _SCALING_MATRICES_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>
#include <cstring>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"
#include "utilities.hpp"
#include "ioutilities.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define SL_4x4_INTRA_Y   0
#define SL_4x4_INTRA_Cb  1
#define SL_4x4_INTRA_Cr  2
#define SL_4x4_INTER_Y   3
#define SL_4x4_INTER_Cb  4
#define SL_4x4_INTER_Cr  5
#define SL_4x4_NUM       6

#define SL_8x8_INTRA_Y   0
#define SL_8x8_INTER_Y   1
#define SL_8x8_INTRA_Cb  2
#define SL_8x8_INTER_Cb  3
#define SL_8x8_INTRA_Cr  4
#define SL_8x8_INTER_Cr  5
#define SL_8x8_NUM       6

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

struct scaling_list_4x4 : public h264_structure
{
    scaling_list_4x4() :
        h264_structure()
    {
    }

    void set_defaults()
    {
        scaling_list_present_flag = 0;
        std::memset(scaling_list, 16, sizeof(scaling_list));
    }

    std::string to_string() const override;

    uint32_t scaling_list_present_flag;
    uint8_t  scaling_list[16];
};

struct scaling_list_8x8 : public h264_structure
{
    scaling_list_8x8() :
        h264_structure()
    {
    }

    void set_defaults()
    {
        scaling_list_present_flag = 0;
        memset(scaling_list, 16, sizeof(scaling_list));
    }

    std::string to_string() const override;

    uint32_t scaling_list_present_flag;
    uint8_t  scaling_list[64];
};

struct scaling_matrices : public h264_structure
{
    scaling_matrices() :
        h264_structure()
    {
    }

    void set_defaults()
    {
        for (std::size_t i = 0; i < TABLE_ELEMENTS(scaling_matrices_4x4); ++i)
            scaling_matrices_4x4[i].set_defaults();

        for (std::size_t i = 0; i < TABLE_ELEMENTS(scaling_matrices_8x8); ++i)
            scaling_matrices_8x8[i].set_defaults();
    }

    std::string to_string() const override;

    scaling_list_4x4 scaling_matrices_4x4[SL_4x4_NUM];
    scaling_list_8x8 scaling_matrices_8x8[SL_8x8_NUM];
};

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

/* Table 7-3 – Specification of default scaling lists Default_4x4_Intra and Default_4x4_Inter */
inline const uint8_t scaling_list_default_4x4[2][16] =
{
    {  6, 13, 20, 28, 13, 20, 28, 32,
      20, 28, 32, 37, 28, 32, 37, 42 },
    { 10, 14, 20, 24, 14, 20, 24, 27,
      20, 24, 27, 30, 24, 27, 30, 34 }
};

/* Table 7-4 – Specification of default scaling lists Default_8x8_Intra and Default_8x8_Inter */
inline const uint8_t scaling_list_default_8x8[2][64] =
{
    {  6, 10, 13, 16, 18, 23, 25, 27,
      10, 11, 16, 18, 23, 25, 27, 29,
      13, 16, 18, 23, 25, 27, 29, 31,
      16, 18, 23, 25, 27, 29, 31, 33,
      18, 23, 25, 27, 29, 31, 33, 36,
      23, 25, 27, 29, 31, 33, 36, 38,
      25, 27, 29, 31, 33, 36, 38, 40,
      27, 29, 31, 33, 36, 38, 40, 42 },
    {  9, 13, 15, 17, 19, 21, 22, 24,
      13, 13, 17, 19, 21, 22, 24, 25,
      15, 17, 19, 21, 22, 24, 25, 27,
      17, 19, 21, 22, 24, 25, 27, 28,
      19, 21, 22, 24, 25, 27, 28, 30,
      21, 22, 24, 25, 27, 28, 30, 32,
      22, 24, 25, 27, 28, 30, 32, 33,
      24, 25, 27, 28, 30, 32, 33, 35 }
};

inline std::string scaling_list_4x4::to_string() const
{
    std::ostringstream stream;

    stream << P3(scaling_list_present_flag);

    if (scaling_list_present_flag) {
        int i = 0;
        while (i < 16) {
            if (!(i % 4)) stream << "        ";
            stream << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(scaling_list[i]) << " ";
            ++i;
            if (!(i % 4)) stream << std::endl;
        }
    }

    return stream.str();
}

inline std::string scaling_list_8x8::to_string() const
{
    std::ostringstream stream;

    stream << P3(scaling_list_present_flag);

    if (scaling_list_present_flag) {
        int i = 0;
        while (i < 64) {
            if (!(i % 8)) stream << "        ";
            stream << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(scaling_list[i]) << " ";
            ++i;
            if (!(i % 8)) stream << std::endl;
        }
    }

    return stream.str();
}

inline std::string scaling_matrices::to_string() const
{
    std::ostringstream stream;

    stream << "  scaling_matrices:" << std::endl;

    stream << "    scaling matrix 4x4 intra Y:" << std::endl;
    stream << scaling_matrices_4x4[SL_4x4_INTRA_Y].to_string();

    stream << "    scaling matrix 4x4 intra Cb:" << std::endl;
    stream << scaling_matrices_4x4[SL_4x4_INTRA_Cb].to_string();

    stream << "    scaling matrix 4x4 intra Cr:" << std::endl;
    stream << scaling_matrices_4x4[SL_4x4_INTRA_Cr].to_string();

    stream << "    scaling matrix 4x4 inter Y:" << std::endl;
    stream << scaling_matrices_4x4[SL_4x4_INTER_Y].to_string();

    stream << "    scaling matrix 4x4 inter Cb:" << std::endl;
    stream << scaling_matrices_4x4[SL_4x4_INTER_Cb].to_string();

    stream << "    scaling matrix 4x4 inter Cr:" << std::endl;
    stream << scaling_matrices_4x4[SL_4x4_INTER_Cr].to_string();

    stream << "    scaling matrix 8x8 intra Y:" << std::endl;
    stream << scaling_matrices_8x8[SL_8x8_INTRA_Y].to_string();

    stream << "    scaling matrix 8x8 inter Y:" << std::endl;
    stream << scaling_matrices_8x8[SL_8x8_INTER_Y].to_string();

    stream << "    scaling matrix 8x8 intra Cb:" << std::endl;
    stream << scaling_matrices_8x8[SL_8x8_INTRA_Cb].to_string();

    stream << "    scaling matrix 8x8 inter Cb:" << std::endl;
    stream << scaling_matrices_8x8[SL_8x8_INTER_Cb].to_string();

    stream << "    scaling matrix 8x8 intra Cr:" << std::endl;
    stream << scaling_matrices_8x8[SL_8x8_INTRA_Cr].to_string();

    stream << "    scaling matrix 8x8 inter Cr:" << std::endl;
    stream << scaling_matrices_8x8[SL_8x8_INTER_Cr].to_string();

    return stream.str();
}

} /* end of namespace h264 */
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

#endif /* _SCALING_MATRICES_HPP_ */
