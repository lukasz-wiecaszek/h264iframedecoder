/**
 * @file hrd_parameters.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 HRD Parameters structure.
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

#ifndef _HRD_PARAMETERS_HPP_
#define _HRD_PARAMETERS_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"
#include "ioutilities.hpp"

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

struct hrd_parameters : public h264_structure
{
    hrd_parameters() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    uint32_t cpb_cnt_minus1;
    uint32_t bit_rate_scale;
    uint32_t cpb_size_scale;
        uint32_t bit_rate_value_minus1[32]; // up to cpb_cnt_minus1, which is <= 31
        uint32_t cpb_size_value_minus1[32];
        uint32_t cbr_flag[32];
    uint32_t initial_cpb_removal_delay_length_minus1;
    uint32_t cpb_removal_delay_length_minus1;
    uint32_t dpb_output_delay_length_minus1;
    uint32_t time_offset_length;
};

inline std::string hrd_parameters::to_string() const
{
    std::ostringstream stream;

    stream << "hrd parameters:" << std::endl;
    stream << P3(cpb_cnt_minus1);
    stream << P3(bit_rate_scale);
    stream << P3(cpb_size_scale);

    for (uint32_t schedSelIdx = 0; schedSelIdx <= cpb_cnt_minus1; ++schedSelIdx) {
        stream << P4(bit_rate_value_minus1[schedSelIdx]);
        stream << P4(cpb_size_value_minus1[schedSelIdx]);
        stream << P4(cbr_flag[schedSelIdx]);
    }

    stream << P3(initial_cpb_removal_delay_length_minus1);
    stream << P3(cpb_removal_delay_length_minus1);
    stream << P3(dpb_output_delay_length_minus1);
    stream << P3(time_offset_length);

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

#endif /* _HRD_PARAMETERS_HPP_ */
