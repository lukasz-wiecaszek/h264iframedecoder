/**
 * @file aspect_ratio_idc.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 aspect ratio indicators.
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

#ifndef _ASPECT_RATIO_IDC_HPP_
#define _ASPECT_RATIO_IDC_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
/* Appendix E, Table E-1 Meaning of sample aspect ratio indicator */
#define ASPECT_RATIO_INDICATORS \
    ASPECT_RATIO_INDICATOR(SAR_Unspecified,   0) \
    ASPECT_RATIO_INDICATOR(SAR_1_1,           1) \
    ASPECT_RATIO_INDICATOR(SAR_12_11,         2) \
    ASPECT_RATIO_INDICATOR(SAR_10_11,         3) \
    ASPECT_RATIO_INDICATOR(SAR_16_11,         4) \
    ASPECT_RATIO_INDICATOR(SAR_40_33,         5) \
    ASPECT_RATIO_INDICATOR(SAR_24_11,         6) \
    ASPECT_RATIO_INDICATOR(SAR_20_11,         7) \
    ASPECT_RATIO_INDICATOR(SAR_32_11,         8) \
    ASPECT_RATIO_INDICATOR(SAR_80_33,         9) \
    ASPECT_RATIO_INDICATOR(SAR_18_11,        10) \
    ASPECT_RATIO_INDICATOR(SAR_15_11,        11) \
    ASPECT_RATIO_INDICATOR(SAR_64_33,        12) \
    ASPECT_RATIO_INDICATOR(SAR_160_99,       13) \
    ASPECT_RATIO_INDICATOR(SAR_4_3,          14) \
    ASPECT_RATIO_INDICATOR(SAR_3_2,          15) \
    ASPECT_RATIO_INDICATOR(SAR_2_1,          16) \
    ASPECT_RATIO_INDICATOR(SAR_EXTENDED,    255) \

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

enum class aspect_ratio_idc_e : int32_t
{
#define ASPECT_RATIO_INDICATOR(id, value) id = value,
    ASPECT_RATIO_INDICATORS
#undef ASPECT_RATIO_INDICATOR
};

constexpr static inline const char* to_string(aspect_ratio_idc_e e)
{
    const char* str = "invalid 'aspect_ratio_idc_e' value";

    switch (e) {
#define ASPECT_RATIO_INDICATOR(id, value) case aspect_ratio_idc_e::id: str = #id; break;
            ASPECT_RATIO_INDICATORS
#undef ASPECT_RATIO_INDICATOR
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

#endif /* _ASPECT_RATIO_IDC_HPP_ */
