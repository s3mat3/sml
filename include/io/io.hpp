/*!
 * \defgroup io IO-util
 * @{
 * \file io.hpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#pragma once

#ifndef IO_Hpp
# define  IO_Hpp

# include "flag.hpp"
# include "debug.hpp"

namespace Sml{
    namespace IO {
        using return_code = Sml::return_code;
        using pos_type    = Sml::count_type; //!< meaning position
        using io_buffer   = std::string;

        static inline constexpr return_code IO_OK          = Sml::OK;
        static inline constexpr return_code IO_FAILURE     = Sml::IO_ERROR_BASE;
        static inline constexpr return_code IO_TIMEOUT     = IO_FAILURE     - 1;
        static inline constexpr return_code IO_CUT_PARTNER = IO_TIMEOUT     - 1;
        static inline constexpr return_code IO_NOT_OPEN    = IO_CUT_PARTNER - 1;
        static inline constexpr return_code IO_SUM_ERROR   = IO_NOT_OPEN    - 1;
        static inline constexpr return_code IO_OVER_RETRY  = IO_SUM_ERROR   - 1;

        using fd_type = int;
        static inline constexpr fd_type void_fd() {return -1;}
        static inline constexpr bool is_error_fd(fd_type fd) {return (fd < 0);}
        static inline constexpr bool is_fd(fd_type fd) {return (fd > 0);}

        enum class direction : int_fast8_t { /// IO direction
          in = 0,
          out = 1,
          in_out = 2,
        };

        } // namespace IO
        } // namespace Sml

#endif //<-- macro  IO_Hpp ends here.
/** @} */
