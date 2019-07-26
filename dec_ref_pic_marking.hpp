/**
 * @file dec_ref_pic_marking.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264
 * Decoded Reference Picture Marking structure.
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

#ifndef _DEC_REF_PIC_MARKING_HPP_
#define _DEC_REF_PIC_MARKING_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"

#include "utilities.hpp"
#include "ioutilities.hpp"
#include "istream.hpp"

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

struct dec_ref_pic_marking_idr : public h264_structure
{
    dec_ref_pic_marking_idr() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    uint32_t no_output_of_prior_pics_flag;
    uint32_t long_term_reference_flag;
};

inline std::string dec_ref_pic_marking_idr::to_string() const
{
    std::ostringstream stream;

    stream << "  decoded reference picture marking (idr):" << std::endl;
    stream << P2(no_output_of_prior_pics_flag);
    stream << P2(long_term_reference_flag);

    return stream.str();
}

struct dec_ref_pic_marking_nonidr : public h264_structure
{
    dec_ref_pic_marking_nonidr() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    uint32_t adaptive_ref_pic_marking_mode_flag;
    struct {
        uint32_t memory_management_control_operation;
        uint32_t difference_of_pic_nums_minus1;
        uint32_t long_term_pic_num;
        uint32_t long_term_frame_idx;
        uint32_t max_long_term_frame_idx_plus1;
    } mmco[4];
};

inline std::string dec_ref_pic_marking_nonidr::to_string() const
{
    std::ostringstream stream;

    stream << "  decoded reference picture marking (non-idr):" << std::endl;

    stream << P2(adaptive_ref_pic_marking_mode_flag);
    if (adaptive_ref_pic_marking_mode_flag) {
        uint32_t memory_management_control_operation;
        std::size_t i = 0;

        do {
            memory_management_control_operation = mmco[i].memory_management_control_operation;

            stream << P3(mmco[i].memory_management_control_operation);

            if (memory_management_control_operation == 1 ||
                memory_management_control_operation == 3)
                stream << P3(mmco[i].difference_of_pic_nums_minus1);

            if (memory_management_control_operation == 2)
                stream << P3(mmco[i].long_term_pic_num);

            if (memory_management_control_operation == 3 ||
                memory_management_control_operation == 6)
                stream << P3(mmco[i].long_term_frame_idx);

            if (memory_management_control_operation == 4)
                stream << P3(mmco[i].max_long_term_frame_idx_plus1);

            i++;
        } while ((i < TABLE_ELEMENTS(mmco)) && (memory_management_control_operation != 0));
    }

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

#endif /* _DEC_REF_PIC_MARKING_HPP_ */
