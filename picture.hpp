/**
 * @file picture.hpp
 *
 * Definition of H.264 (ISO/IEC 14496-10) picture structure.
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

#ifndef _PICTURE_HPP_
#define _PICTURE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "picture_structure.hpp"
#include "slice_header.hpp"
#include "slice_data.hpp"
#include "colour_component.hpp"
#include "mb.hpp"
#include "mb_cache.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

class h264_decoder; // forward declaration

namespace h264
{

class picture
{
public:
    explicit picture(const h264_decoder& decoder, const h264::slice_header& sh);
    virtual ~picture();

    virtual void decode(const h264::slice_header& sh, const h264::slice_data& sd) = 0;

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << std::endl;

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

protected:
    void init_coxtext_variables(const h264::slice_header& sh);
    bool is_mb_available(int n);
    mb* get_mb(int n);
    mb* curr_mb();
    void calculate_neighbours_part1();
    void calculate_neighbours_part2();
    void advance_mb_pos();

protected:
    struct context_variables
    {
        bool mb_aff_frame;
        bool mb_field_decoding_flag;
        int mb_x;
        int mb_y;
        int mb_pos;
        mb* curr_mb;
        int lastQPdelta;
        int QPy;
        int QPc[2];
        int chroma_array_type;

        const uint8_t* left_blocks;

        mb_cache intraNxN_pred_mode;
        mb_cache non_zero_count[COLOUR_COMPONENTS_MAX];
    };

    const h264_decoder& m_decoder;
    picture_structure_e m_picture_structure;
    context_variables m_context_variables;
    h264::mb* m_mbs;
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

/*
  The macroblock is marked as available,
  unless any of the following conditions are true,
  in which case the macroblock is marked as not available:
   - mbAddr < 0,
   - mbAddr > CurrMbAddr,
   - the macroblock with address mbAddr belongs to a different slice
  than the macroblock with address CurrMbAddr.
*/
inline bool picture::is_mb_available(int n)
{
    return ((n >= 0) && (n <= m_context_variables.mb_pos));
}

inline mb* picture::get_mb(int n)
{
    return is_mb_available(n) ? &m_mbs[n] : nullptr;
}

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

#endif /* _PICTURE_HPP_ */
