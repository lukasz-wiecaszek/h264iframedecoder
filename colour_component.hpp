/**
 * @file colour_component.hpp
 *
 * Provides definitions of picture colour components.
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

#ifndef _COLOUR_COMPONENT_HPP_
#define _COLOUR_COMPONENT_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define COLOUR_COMPONENTS \
    COLOUR_COMPONENT(Y,  0) \
    COLOUR_COMPONENT(Cb, 1) \
    COLOUR_COMPONENT(Cr, 2) \

#define COLOUR_COMPONENTS_MAX 3

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

enum class colour_component_e : int32_t
{
#define COLOUR_COMPONENT(id, value) id = value,
    COLOUR_COMPONENTS
#undef COLOUR_COMPONENT
};

constexpr static inline const char* to_string(colour_component_e e)
{
    const char* str = "invalid 'colour_component_e' value";

    switch (e) {
#define COLOUR_COMPONENT(id, value) case colour_component_e::id: str = #id; break;
        COLOUR_COMPONENTS
#undef COLOUR_COMPONENT
    }

    return str;
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

#endif /* _COLOUR_COMPONENT_HPP_ */
