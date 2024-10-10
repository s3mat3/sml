/*!
 * \file status_flag.hpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#pragma once

#ifndef STATUS_FLAG_Hpp
# define  STATUS_FLAG_Hpp
# include "flag.hpp"
# include "io/io.hpp"
// The device channel has connector.
namespace Sml {
    namespace IO {
        /*! IO status flag .
         */
        struct StatusFlag : public Sml::FlagRegister
        {
        public:
            using Sml::FlagRegister::FlagRegister;

            static constexpr bit_pattern_type nosetup     = FlagRegister::zero;    //!< 0x0000'0000
            static constexpr bit_pattern_type opened      = FlagRegister::shl(0);  //!< 0x0000'0001
            static constexpr bit_pattern_type accepted    = FlagRegister::shl(1);  //!< 0x0000'0002
            static constexpr bit_pattern_type connected   = FlagRegister::shl(1);  //!< 0x0000'0002
            static constexpr bit_pattern_type listening   = FlagRegister::shl(2);  //!< 0x0000'0004
            static constexpr bit_pattern_type ready_read  = FlagRegister::shl(3);  //!< 0x0000'0008
            static constexpr bit_pattern_type ready_write = FlagRegister::shl(4);  //!< 0x0000'0010
            static constexpr bit_pattern_type reseted     = FlagRegister::shl(5);  //!< 0x0000'0020
            static constexpr bit_pattern_type nolistener  = FlagRegister::shl(6);  //!< 0x0000'0040
            // error status
            static constexpr bit_pattern_type failure     = FlagRegister::shl(16); //!< 0x0001'0000
            static constexpr bit_pattern_type timeouted   = FlagRegister::shl(17); //!< 0x0002'0000

            StatusFlag(bit_pattern_type t) : FlagRegister(t) {}
            StatusFlag() : StatusFlag(StatusFlag::nosetup) {}
            ~StatusFlag() = default;
            StatusFlag(const StatusFlag&) = delete;
            StatusFlag(StatusFlag&&) noexcept = delete;
            operator bool() {return is_set(StatusFlag::opened);}
            void reset_error() {reset((StatusFlag::failure | StatusFlag::timeouted));}
        }; //<-- class StatusFlag ends here.
    } //<-- namespace IO ends here.
} //<-- namespace Sml ends here.

#endif //<-- macro  STATUS_FLAG_Hpp ends here.
