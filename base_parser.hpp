/**
 * @file base_parser.hpp
 *
 * Definition of generic parser design pattern.
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

#ifndef _BASE_PARSER_HPP_
#define _BASE_PARSER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

#include <cstring>
#include <climits>

#if defined(DEBUG)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "flatbuffer.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

template<typename T>
class base_parser
{
public:
    typedef T value_type;

    explicit base_parser(std::size_t capacity) :
        m_flatbuffer(capacity),
        m_user_data(nullptr)
    {
#if defined(DEBUG)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << to_string() << std::endl;
#endif
    }

    virtual ~base_parser()
    {
#if defined(DEBUG)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    base_parser(const base_parser&) = delete;
    base_parser(base_parser&&) = delete;
    base_parser& operator = (const base_parser&) = delete;
    base_parser& operator = (base_parser&&) = delete;

    void reset()
    {
        m_flatbuffer.reset();
        m_user_data = nullptr;
    }

    void clear_buffer()
    {
        m_flatbuffer.reset();
    }

    void set_user_data(void* user_data)
    {
        m_user_data = user_data;
    }

    void* get_user_data() const
    {
        return m_user_data;
    }

    std::size_t write(const T data[], std::size_t count)
    {
        if (count > m_flatbuffer.write_available())
            m_flatbuffer.move();

        return m_flatbuffer.write(data, count);
    }

    std::size_t write(const T& data)
    {
        return write(&data, 1);
    }

    template<std::size_t N>
    std::size_t write(const T (&data)[N])
    {
        return write(data, N);
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << "base_parser@";
        stream << std::hex << this;
        stream << std::endl;
        stream << m_flatbuffer.to_string();

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

protected:
    flatbuffer<T> m_flatbuffer;

private:
    void* m_user_data;
};

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

#endif /* _BASE_PARSER_HPP_ */
