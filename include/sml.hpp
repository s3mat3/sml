/*!
 * \file sml.hpp
 *
 * \copyright © 2024 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief S3mat3's Mini Library for tiny develop
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_Hpp
# define  SML_Hpp

# include <cstdint>
# include <cstring> // for ::strerror_r
# include <limits>
# include <string>
# include <type_traits>

# define SML_SGR_RESET   "\e[0m"
# define SML_SGR_BOLD    "\e[1m"
# define SML_SGR_THIN    "\e[2m"
# define SML_SGR_ITALIC  "\e[3m"
# define SML_SGR_UNDER   "\e[4m"
# define SML_SGR_BLACK   "\e[30m"
# define SML_SGR_RED     "\e[31m"
# define SML_SGR_GREEN   "\e[32m"
# define SML_SGR_YELLOW  "\e[33m"
# define SML_SGR_BLUE    "\e[34m"
# define SML_SGR_MAGENTA "\e[35m"
# define SML_SGR_CYAN    "\e[36m"
# define SML_SGR_WHITE   "\e[37m"


# define SML_UNUSED_ARG(x) static_cast<void>(x)


# define SML_NAMESPACE_BEGIN namespace Sml {
# define SML_NAMESPACE_END }
using namespace std::literals::string_literals;

namespace Sml {
    using void_ptr          = void*;
    using errno_t           = int;
    using byte_type         = char;
    using return_code       = std::int64_t; //!< return code
    using logic_code        = std::int64_t;
    using count_type        = std::size_t;
    using index_type        = std::size_t;
    using size_type         = std::size_t;
    using ID_t              = std::size_t;
    using ByteArray_t       = std::string;
    using time_interval     = std::int32_t;
    using millisec_interval = time_interval; //!< ミリ秒 2^31=2147483648[ms]で約2147483[s]で約596時間で約24日が最大となる
    using seconds_interval  = time_interval; //!< 秒 2^31=2147483648[s]で約596523時間で約24855日が最大となる
    using name_type         = std::string;

    /*! maximum value of index_t */
    constexpr index_type max_index() {return std::numeric_limits<index_type>::max();}
    /*! minimum value of index_t */
    constexpr index_type min_index() {return std::numeric_limits<index_type>::min();}
    /*! maximum value of return_code */
    constexpr return_code max_return_code() {return std::numeric_limits<return_code>::max();}
    /*! minimum value of return_code */
    constexpr return_code min_return_code() {return std::numeric_limits<return_code>::min();}
    /*!  return_code .
     *
     *
     */
    static constexpr return_code DEVICE_ERROR_BASE = -30000;
    static constexpr return_code IO_ERROR_BASE     = -20000;
    static constexpr return_code OVER_INDEX        = -10104;
    static constexpr return_code UNDER_INDEX       = -10103;
    static constexpr return_code OVER_FLOW         = -10102;
    static constexpr return_code UNDER_FLOW        = -10101;
    static constexpr return_code OUT_OF_RANGE      = -10100;
    static constexpr return_code FAIL_JOIN         = -10014;
    static constexpr return_code FAIL_LUNCH        = -10013;
    static constexpr return_code FAIL_CMD          = -10012;
    static constexpr return_code FAIL_ARGC         = -10011;
    static constexpr return_code FAIL_ARG          = -10010;
    static constexpr return_code NO_DATA           = -10002;
    static constexpr return_code NO_RESOURCE       = -10001;
    static constexpr return_code TIMEOUT           = -10000;
    static constexpr return_code FAILURE           = -1;
    static constexpr return_code OK                =  0;

    /*! code class .
     *
     * The code generator
     */
    template <typename T>
    struct code
    {
        ~code()               = default;
        code()                = default;
        constexpr code(const code&)     = default;
        constexpr code(code&&) noexcept = default;
        constexpr explicit code(T v) : m_value(v) {}
        constexpr operator T() const noexcept {return m_value;}
        const T m_value {};
    };

    struct return_values : public code<return_code>
    {
        return_values() = default;
        constexpr return_values(const return_code c) : code {c} {}
        constexpr return_values(const return_code& c) : code {c} {}
        static constexpr code ok {0};
        static constexpr code failure {0xffff'ffff};
        static constexpr code timeout {0xffff'f001};
    };

    static constexpr bool enable  = true;
    static constexpr bool disable = false;
    /*! Logic code
     *
     *
     */
    enum class LogicCode : logic_code {
        UNK = -1, //!< unknown state
        OFF =  0, //!< off state
        ON  =  1, //!< on state
        HiZ =  2, //!< high impedance state
    };
    static inline constexpr logic_code convert(LogicCode c) {return static_cast<logic_code>(c);}
    static inline constexpr LogicCode OFF()     {return LogicCode::OFF;}
    static inline constexpr LogicCode nOFF()    {return LogicCode::ON;}
    static inline constexpr LogicCode ON()      {return LogicCode::ON;}
    static inline constexpr LogicCode nON()     {return LogicCode::OFF;}
    static inline constexpr LogicCode Unknown() {return LogicCode::UNK;}
    static inline constexpr LogicCode HiZ()     {return LogicCode::HiZ;}

    /*! char type checker .
     *
     *
     */
    template<typename T>
    struct is_char {static constexpr bool value = false;};

    template<>
    struct is_char<char> {static constexpr bool value = true;};

    template<typename T>
    static constexpr bool is_char_v = is_char<T>::value;
    /*! range checker .
     *
     *  \tparam lower meaning start of range for storage
     *  \tparam upper meaning end of range for storage
     *  \tparam target is position for storage to be check
     */
    template<size_type lower, size_type upper, size_type target>
    struct static_range_check {
        static constexpr bool value = (lower <= target && target < upper);
    };

    template<size_type l, size_type u, size_type t>
    static constexpr bool static_range_check_v = static_range_check<l,u,t>::value;

    //
    // util functions
    //
    
    /*! Extract file name from full path.
     *
     *  Normal function version
     */
    inline std::string removePath(const std::string& target, char del = '/')
    {
        auto pos = target.find_last_of(del);
        if (pos != std::string::npos) {
            return target.substr(pos + 1);
        }
        return target;
    }

    inline auto system_error(int err) -> std::string
    {
        std::string cause;
        cause.assign(::strerror_r(err, cause.data(), 128));
        return cause;
    }


} //<-- namespace Sml ends here.

#endif //<-- macro  SML_Hpp ends here.
