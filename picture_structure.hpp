/**
 * @file picture_structure.hpp
 *
 * Definition of picture structure enumeration type.
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

#ifndef _PICTURE_STRUCTURE_HPP_
#define _PICTURE_STRUCTURE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdint>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define PICTURE_STRUCTURES \
    PICTURE_STRUCTURE(frame) \
    PICTURE_STRUCTURE(field_top) \
    PICTURE_STRUCTURE(field_bottom) \

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

enum class picture_structure_e : int32_t
{
#define PICTURE_STRUCTURE(id) id,
    PICTURE_STRUCTURES
#undef PICTURE_STRUCTURE
};

constexpr static inline const char* to_string(picture_structure_e e)
{
    const char* str = "invalid 'picture_structure_e' value";

    switch (e) {
#define PICTURE_STRUCTURE(id) case picture_structure_e::id: str = #id; break;
            PICTURE_STRUCTURES
#undef PICTURE_STRUCTURE
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

#endif /* _PICTURE_STRUCTURE_HPP_ */
