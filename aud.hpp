/**
 * @file aud.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 Access Unit Delimiter structure.
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

#ifndef _AUD_HPP_
#define _AUD_HPP_

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

constexpr static inline const char* primary_pic_type_to_string(uint32_t primary_pic_type)
{
    const char* str = "invalid 'primary_pic_type' value";

    switch (primary_pic_type) {
        case 0: str = "(I)"; break;
        case 1: str = "(I, P)"; break;
        case 2: str = "(I, P, B)"; break;
        case 3: str = "(SI)"; break;
        case 4: str = "(SI, SP)"; break;
        case 5: str = "(I, SI)"; break;
        case 6: str = "(I, SI, P, SP)"; break;
        case 7: str = "(I, SI, P, SP, B)"; break;
    }

    return str;
}

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

struct aud : public h264_structure
{
    aud() :
        h264_structure()
    {
    }

    std::string to_string() const override;

    uint32_t primary_pic_type;
};

inline std::string aud::to_string() const
{
    std::ostringstream stream;

    stream << "aud (access unit delimiter):" << std::endl;
    stream << P1_INFO(primary_pic_type, primary_pic_type_to_string(primary_pic_type));

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

#endif /* _AUD_HPP_ */
