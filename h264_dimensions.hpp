/**
 * @file h264_dimensions.hpp
 *
 * h264 dimensions.
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

#ifndef _H264_DIMENSIONS_HPP_
#define _H264_DIMENSIONS_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "sps.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

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

struct h264_dimensions
{
    h264_dimensions() :
        mb_width{0},
        mb_height{0},
        mb_num{0},
        width{0},
        height{0}
    {
    }

    void reset(const h264::sps& sps)
    {
        mb_width  = (sps.pic_width_in_mbs_minus1 + 1);
        mb_height = (sps.pic_height_in_map_units_minus1 + 1) * (2 - sps.frame_mbs_only_flag);
        mb_num    = mb_width * mb_height;

        width = 16 * mb_width;
        height = 16 * mb_height;
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << "dimensions:" << std::endl;
        stream << P1(mb_width);
        stream << P1(mb_height);
        stream << P1(mb_num);
        stream << P1(width);
        stream << P1(height);

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    int mb_width;
    int mb_height;
    int mb_num;

    int width;
    int height;
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

#endif /* _H264_DIMENSIONS_HPP_ */
