/**
 * @file picture_cavlc.hpp
 *
 * Definition of H.264 (ISO/IEC 14496-10) picture structure.
 *
 * Entropy decoding method to be applied for the syntax elements
 * for which two descriptors appear in the syntax tables is either
 * Exp-Golomb (see subclause 9.1) or CAVLC (see subclause 9.2).

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

#ifndef _PICTURE_CAVLC_HPP_
#define _PICTURE_CAVLC_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "picture.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

namespace h264
{

class picture_cavlc : public picture
{
public:
    explicit picture_cavlc(const h264_decoder& decoder, const h264::slice_header& sh);
    ~picture_cavlc() override;

    void decode(const h264::slice_header& sh, const h264::slice_data& sd) override;
};

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

} /* end of namespace h264 */
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

#endif /* _PICTURE_CAVLC_HPP_ */
