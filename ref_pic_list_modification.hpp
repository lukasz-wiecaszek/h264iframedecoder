/**
 * @file ref_pic_list_modification.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 Reference Picture List
 * Modification structure.
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

#ifndef _REF_PIC_LIST_MODIFICATION_HPP_
#define _REF_PIC_LIST_MODIFICATION_HPP_

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

struct ref_pic_list_modification : public h264_structure
{
    ref_pic_list_modification() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    uint32_t ref_pic_list_modification_flag_lx;
    struct {
        uint32_t modification_of_pic_nums_idc;
        union {
            uint32_t abs_diff_pic_num_minus1;
            uint32_t long_term_pic_num;
        } u;
    } m[4];
};

inline std::string ref_pic_list_modification::to_string() const
{
    std::ostringstream stream;

    stream << "  reference picture list modification:" << std::endl;

    stream << P2(ref_pic_list_modification_flag_lx);
    if (ref_pic_list_modification_flag_lx) {
        uint32_t modification_of_pic_nums_idc;
        std::size_t i = 0;

        do {
            modification_of_pic_nums_idc = m[i].modification_of_pic_nums_idc;

            stream << P3(m[i].modification_of_pic_nums_idc);

            if (modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1)
                stream << P3(m[i].u.abs_diff_pic_num_minus1);
            else
            if (modification_of_pic_nums_idc == 2)
                stream << P3(m[i].u.long_term_pic_num);

            i++;
        } while ((i < TABLE_ELEMENTS(m)) && (modification_of_pic_nums_idc != 3));
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

#endif /* _REF_PIC_LIST_MODIFICATION_HPP_ */
