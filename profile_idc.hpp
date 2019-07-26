/**
 * @file profile_idc.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 profiles.
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

#ifndef _PROFILE_IDC_HPP_
#define _PROFILE_IDC_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define PROFILE_INDICATORS \
    PROFILE_INDICATOR(UNKNOWN,            0) \
    PROFILE_INDICATOR(BASELINE,          66) \
    PROFILE_INDICATOR(MAIN,              77) \
    PROFILE_INDICATOR(EXTENDED,          88) \
    PROFILE_INDICATOR(HIGH,             100) \
    PROFILE_INDICATOR(HIGH10,           110) \
    PROFILE_INDICATOR(HIGH422,          122) \
    PROFILE_INDICATOR(HIGH444,          144) \
    PROFILE_INDICATOR(CAVLC444,         244) \
    PROFILE_INDICATOR(SCALABLE_BASELINE, 83) \
    PROFILE_INDICATOR(SCALABLE_HIGH,     86) \

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

enum class profile_idc_e : int32_t
{
#define PROFILE_INDICATOR(id, value) id = value,
    PROFILE_INDICATORS
#undef PROFILE_INDICATOR
};

constexpr static inline const char* to_string(profile_idc_e e)
{
    const char* str = "invalid 'profile_idc_e' value";

    switch (e) {
#define PROFILE_INDICATOR(id, value) case profile_idc_e::id: str = #id; break;
            PROFILE_INDICATORS
#undef PROFILE_INDICATOR
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

#endif /* _PROFILE_IDC_HPP_ */
