/**
 * @file chroma_format.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 chroma formats.
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

#ifndef _CHROMA_FORMAT_HPP_
#define _CHROMA_FORMAT_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define CHROMA_FORMATS \
    CHROMA_FORMAT(monochrome,      0) \
    CHROMA_FORMAT(pattern_4_2_0,   1) \
    CHROMA_FORMAT(pattern_4_2_2,   2) \
    CHROMA_FORMAT(pattern_4_4_4,   3) \

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

enum class chroma_format_e : int32_t
{
#define CHROMA_FORMAT(id, value) id = value,
    CHROMA_FORMATS
#undef CHROMA_FORMAT
};

constexpr static inline const char* to_string(chroma_format_e e)
{
    const char* str = "invalid 'chroma_format_e' value";

    switch (e) {
#define CHROMA_FORMAT(id, value) case chroma_format_e::id: str = #id; break;
            CHROMA_FORMATS
#undef CHROMA_FORMAT
    }

    return str;
}

constexpr static inline bool operator == (uint32_t v, chroma_format_e c)
{
    return v == static_cast<uint32_t>(c);
}

constexpr static inline bool operator == (chroma_format_e c, uint32_t v)
{
    return v == c;
}

constexpr static inline bool operator != (uint32_t v, chroma_format_e c)
{
    return !(v == c);
}

constexpr static inline bool operator != (chroma_format_e c, uint32_t v)
{
    return !(c == v);
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

#endif /* _CHROMA_FORMAT_HPP_ */
