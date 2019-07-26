/**
 * @file slice_type.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 slice types.
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

#ifndef _SLICE_TYPE_HPP_
#define _SLICE_TYPE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define SLICE_TYPES \
    SLICE_TYPE(P,  0) \
    SLICE_TYPE(B,  1) \
    SLICE_TYPE(I,  2) \
    SLICE_TYPE(SP, 3) \
    SLICE_TYPE(SI, 4) \

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

enum class slice_type_e : int32_t
{
#define SLICE_TYPE(id, value) id = value,
    SLICE_TYPES
#undef SLICE_TYPE
};

constexpr static inline const char* to_string(slice_type_e e)
{
    const char* str = "invalid 'slice_type_e' value";

    switch (e) {
#define SLICE_TYPE(id, value) case slice_type_e::id: str = #id; break;
            SLICE_TYPES
#undef SLICE_TYPE
    }

    return str;
}

constexpr static inline slice_type_e to_slice_type_e(uint32_t v)
{
    return static_cast<slice_type_e>(v % 5);
}

constexpr static inline bool operator == (uint32_t v, slice_type_e s)
{
    return to_slice_type_e(v) == s;
}

constexpr static inline bool operator == (slice_type_e s, uint32_t v)
{
    return v == s;
}

constexpr static inline bool operator != (uint32_t v, slice_type_e s)
{
    return !(v == s);
}

constexpr static inline bool operator != (slice_type_e s, uint32_t v)
{
    return !(s == v);
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

#endif /* _SLICE_TYPE_HPP_ */
