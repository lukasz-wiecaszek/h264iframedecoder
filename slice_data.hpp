/**
 * @file slice_data.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 slice data structure.
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

#ifndef _SLICE_DATA_HPP_
#define _SLICE_DATA_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdint>
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

struct slice_data : public h264_structure
{
    slice_data() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    const uint8_t* data;
    size_t size;
    size_t bit_pos;
};

inline std::string slice_data::to_string() const
{
    std::ostringstream stream;

    stream << "slice_data:" << std::endl;

    stream << P1(size);
    stream << P1(bit_pos);

    return stream.str();
}

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
} /* end of namespace ymn */

#endif /* _SLICE_DATA_HPP_ */
