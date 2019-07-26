/**
 * @file nal_unit_type.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 and RFC 6184 NAL unit types.
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

#ifndef _NAL_UNIT_TYPE_HPP_
#define _NAL_UNIT_TYPE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define NAL_UNIT_TYPES \
    NAL_UNIT_TYPE(UNSPECIFIED,                    0) \
    NAL_UNIT_TYPE(CODED_SLICE_NON_IDR_PICTURE,    1) \
    NAL_UNIT_TYPE(CODED_SLICE_DATA_PARTITION_A,   2) \
    NAL_UNIT_TYPE(CODED_SLICE_DATA_PARTITION_B,   3) \
    NAL_UNIT_TYPE(CODED_SLICE_DATA_PARTITION_C,   4) \
    NAL_UNIT_TYPE(CODED_SLICE_IDR_PICTURE,        5) \
    NAL_UNIT_TYPE(SEI,                            6) \
    NAL_UNIT_TYPE(SPS,                            7) \
    NAL_UNIT_TYPE(PPS,                            8) \
    NAL_UNIT_TYPE(AUD,                            9) \
    NAL_UNIT_TYPE(END_OF_SEQUENCE,               10) \
    NAL_UNIT_TYPE(END_OF_STREAM,                 11) \
    NAL_UNIT_TYPE(FILLER_DATA,                   12) \
    NAL_UNIT_TYPE(SPS_EXTENSION,                 13) \
    NAL_UNIT_TYPE(PREFIX_NAL_UNIT,               14) \
    NAL_UNIT_TYPE(SPS_SUBSET,                    15) \
    NAL_UNIT_TYPE(RESERVED1,                     16) \
    NAL_UNIT_TYPE(RESERVED2,                     17) \
    NAL_UNIT_TYPE(RESERVED3,                     18) \
    NAL_UNIT_TYPE(CODED_SLICE_AUXILIARY_PICTURE, 19) \
    NAL_UNIT_TYPE(CODED_SLICE_EXTENSION,         20) \
    NAL_UNIT_TYPE(RESERVED4,                     21) \
    NAL_UNIT_TYPE(RESERVED5,                     22) \
    NAL_UNIT_TYPE(RESERVED6,                     23) \
    NAL_UNIT_TYPE(STAP_A,                        24) \
    NAL_UNIT_TYPE(STAP_B,                        25) \
    NAL_UNIT_TYPE(MTAP16,                        26) \
    NAL_UNIT_TYPE(MTAP24,                        27) \
    NAL_UNIT_TYPE(FU_A,                          28) \
    NAL_UNIT_TYPE(FU_B,                          29) \
    NAL_UNIT_TYPE(RESERVED7,                     30) \
    NAL_UNIT_TYPE(RESERVED8,                     31) \

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

enum class nal_unit_type_e : int32_t
{
#define NAL_UNIT_TYPE(id, value) id = value,
    NAL_UNIT_TYPES
#undef NAL_UNIT_TYPE
};

constexpr static inline const char* to_string(nal_unit_type_e e)
{
    const char *str = "invalid 'nal_unit_type_e' value";

    switch (e) {
#define NAL_UNIT_TYPE(id, value) case nal_unit_type_e::id: str = #id; break;
            NAL_UNIT_TYPES
#undef NAL_UNIT_TYPE
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

#endif /* _NAL_UNIT_TYPE_HPP_ */
