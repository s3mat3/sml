/*!
 * \file debug.hpp
 *
 * \copyright © 2024 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Utilty for print debug
 *
 * compile option -Dxxx using.
 * - Assertion
 *  - When set SML_ASSERT_CHECK and unset SML_DEBUG_DISABLE then valid SML_ASSERT macro.
 * - Function name
 *  - When set SML_TRACE and unset SML_DEBUG_DISABLE then valid TRACE, TYPE, MSG, TEXT, DUMP and PTR_GAP macros.
 * - Logging
 *  - When unset SML_LOG_DISABLE then valid SML_FATAL,ERROR,WARN,NOTICE,INFO,LOG, VAR_DUMP and MARK macros.
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_DEBUG_Hpp
# define  SML_DEBUG_Hpp

# include <cxxabi.h>
# include <array>
# include <chrono>
# include <iomanip>
# include <iostream>
# include <mutex>
# include <sstream>
# include <stdexcept>
# include <sys/time.h>
# include <thread>

# include "sml.hpp"

// for simple test macro
static inline std::mutex sml_test_locker_DEBUG_HPP;

# define SML_TEST_BEGIN(func,...) void func(__VA_ARGS__) {              \
    const char* sml_test_function_name_DEBUG_HPP = #func;               \
    {                                                                   \
        std::lock_guard<std::mutex> lock(sml_test_locker_DEBUG_HPP);    \
        std::clog << SML_SGR_BOLD                                       \
                  << "===== TEST "                                      \
                  << sml_test_function_name_DEBUG_HPP                   \
                  << " BEGIN ====="                                     \
                  << SML_SGR_RESET                                      \
                  << std::endl;                                         \
    }

# define SML_TEST_END     {                                             \
        std::lock_guard<std::mutex> lock(sml_test_locker_DEBUG_HPP);    \
        std::clog << SML_SGR_BOLD                                       \
                  << "===== TEST "                                      \
                  << sml_test_function_name_DEBUG_HPP                   \
                  << " END ====="                                       \
                  << SML_SGR_RESET                                      \
                  << std::endl;                                         \
    }}

# if defined (SML_ASSERT_CHECK) && !defined (SML_DEBUG_DISABLE)
#define SML_ASSERT(cond, msg, flg)                                      \
    do {                                                                \
        const char* cnd = #cond;                                        \
        if (!(cond)) {                                                  \
            Sml::Debug::assertion(cnd, msg, flg, Sml::Debug::removePath(__FILE__), __PRETTY_FUNCTION__, __LINE__); \
        } else {                                                        \
            Sml::Debug::assertion_ok(cnd, Sml::Debug::removePath(__FILE__), __PRETTY_FUNCTION__, __LINE__); \
        }                                                               \
    } while (0)
#define SML_CHECK(cond)                                                 \
    do {                                                                \
        const char* cnd = #cond;                                        \
        if (!(cond)) {                                                  \
            Sml::Debug::check(cnd, Sml::Debug::removePath(__FILE__), __PRETTY_FUNCTION__, __LINE__); \
        } else {                                                        \
            Sml::Debug::check_ok(cnd, Sml::Debug::removePath(__FILE__), __PRETTY_FUNCTION__, __LINE__); \
        }                                                               \
    } while (0)
# else
#  define SML_ASSERT(cond, msg, flg)
# endif

# if !defined (SML_LOG_DISABLE)
#  define MARK() Sml::Debug::out_message(Sml::Debug::mark(Sml::Debug::removePath(__FILE__), __LINE__ ))
#  define VAR_DUMP(v) Sml::Debug::dump_str((#v), v)
#  define SML_FATAL(msg) Sml::Debug::out_message(Sml::Debug::build_log_message(msg, Sml::Debug::removePath(__FILE__), __LINE__,0))
#  define SML_ERROR(msg) Sml::Debug::out_message(Sml::Debug::build_log_message(msg, Sml::Debug::removePath(__FILE__), __LINE__,1))
#  define SML_WARN(msg) Sml::Debug::out_message(Sml::Debug::build_log_message(msg, Sml::Debug::removePath(__FILE__), __LINE__,2))
#  define SML_NOTICE(msg) Sml::Debug::out_message(Sml::Debug::build_log_message(msg, Sml::Debug::removePath(__FILE__), __LINE__, 3))
#  define SML_INFO(msg) Sml::Debug::out_message(Sml::Debug::build_log_message(msg, Sml::Debug::removePath(__FILE__), __LINE__, 4))
#  define SML_LOG(msg) Sml::Debug::out_message(Sml::Debug::build_log_message(msg, Sml::Debug::removePath(__FILE__), __LINE__, 5))
# else
#  define MARK()
#  define VAR_DUMP(v)
#  define SML_FATAL(msg)
#  define SML_ERROR(msg)
#  define SML_WARN(msg)
#  define SML_NOTICE(msg)
#  define SML_INFO(msg)
#  define SML_LOG(msg)
# endif

# if defined (SML_TRACE) && !defined (SML_DEBUG_DISABLE)
#  define TRACE(msg) Sml::Debug::out_message(Sml::Debug::trace(msg, __PRETTY_FUNCTION__, __LINE__))
#  define TYPE(var) Sml::Debug::type(var)
#  define MSG(msg) Sml::Debug::out_message(Sml::Debug::message(msg, __LINE__))
#  define TEXT(v) (#v)
#  define DUMP(v) Sml::Debug::dump(TEXT(v), v)
#  define PTR_GAP(ptop,pend) Sml::Debug::out_message(Sml::Debug::size_gap(ptop, pend, #pend, #ptop, false))
# else
#  define TRACE(msg)
#  define TYPE(var)
#  define MSG(msg)
#  define TEXT(v)
#  define DUMP(v)
#  define PTR_GAP(ptop,pend)
# endif

namespace Sml {
    namespace Debug {
        std::string toReadableCtrlCode(const std::string&);
        /*! Type name demangling .
         *
         * Usage
         * \code
         * std::cout << "Type name is " << Sml::Debug::demangle(typeid(size_t).name()) << std::endl;
         * \endcode
         */
        inline std::string demangle(const char* name)
        {
            int status = 0;
            auto x = abi::__cxa_demangle(name, 0, 0, &status); // x is malloced in __cxa_demangle function copy aferter free
            std::string y(x);
            //////////////////////////////////////////////////
            free(x); // free x  *** This line don't remove ***
            //////////////////////////////////////////////////
            return y;
        }
        static inline std::mutex clog_locker_global;
        /*! Output message to system opend clog (stderr with stream buffer).
         *
         * With resource bariyer
         */
        inline  auto out_message(const std::string& msg) -> void
        {
            std::lock_guard<std::mutex> lock(clog_locker_global);
            std::clog << msg << std::endl;
        }

        /*! Extract file name from full path.
         *
         * constexpr version
         * nessesary target is fixed (like __FILE__ macro)
         */
        static inline constexpr std::string_view removePath(const char* target, char del='/')
        {
            std::string_view x = target;
            return x.substr(x.find_last_of(del) + 1);
        }
        /*! Get current time. 
         *
         * \note Only POSIX, precision microseconds
         */
        // [[deprecated("Please using another function!! This is only POSIX system")]]
        inline std::string getCurrentTime()
        {
            std::string buf;
            std::string timeStampFormat = "0000-00-00T00:00:00"; // iso8601 style
            struct timeval now;
            gettimeofday(&now, NULL);
            // std::string usec = std::to_string( now.tv_usec );
            std::stringstream usec;
            usec << std::setw(6) << std::setfill('0') << std::to_string(now.tv_usec);
            std::strftime(const_cast<char *>(timeStampFormat.c_str()),
                          timeStampFormat.size() + 1, "%FT%H:%M:%S",
                          std::localtime(&(now.tv_sec)));
            buf = timeStampFormat;
            buf += ".";
            buf += usec.str();
            return buf;
        }
        /*! Get elapsed time form startup .
         *
         * Get the current elapsed time in microseconds since the start of startup
         *
         * In debian/gcc, it seems that the start of the OS is set as epoch, and in other environments, the start of the target application may be set as epoch (not a big problem).
         */
        inline auto getElapsedTime() -> std::time_t
        {
            using namespace std::chrono;
            return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
        }
        /*! Convert std::chrono::time_t to string .
         *
         *  Format of 16 digits right-justified with 0 padding by omitting the second argument
         *
         *  \param[in] target is conversion target
         *  \param[in] fill is column size, default 16column
         *  \retval string with fill digits right justified and 0 padding
         */
        inline auto convertTime_tToStr(std::time_t target, size_t fill=16) -> std::string
        {
            std::stringstream time_stream;
            time_stream << std::setw(fill) << std::setfill('0') << target;
            return time_stream.str();
        }
        /*! Check std::string.
         *
         * When T == std::string then value is true. 
         */
        template <typename T>
        struct is_string {static constexpr bool value = false;};
        template <>
        struct is_string<std::string> {static constexpr bool value = true;};
        template <typename T>
        constexpr bool is_string_v = is_string<T>::value; //!< alias for is_string::value
        /*! Check std::string_view.
         *
         * WHen T == std::string_view then value is true.
         */
        template <typename T>
        struct is_string_view {static constexpr bool value = false;};
        template <>
        struct is_string_view<std::string_view> {static constexpr bool value = true;};
        template <typename T>
        constexpr bool is_string_view_v = is_string_view<T>::value; //!< alias for is_string_view::value
        /*! Traits of the C language string .
         *
         *
         */
        template <typename T>
        using is_c_string = std::disjunction<
            std::is_same<typename std::decay<T>::type, char*>
            , std::is_same<typename std::decay<T>::type, char const*>
            , std::is_same<typename std::decay<T>::type, signed char*>
            , std::is_same<typename std::decay<T>::type, signed char const*>
            , std::is_same<typename std::decay<T>::type, unsigned char*>
            , std::is_same<typename std::decay<T>::type, unsigned char const*>
            >;
        /*! Has to_string function in class member .
         *
         *
         */
        struct has_to_string_impl
        {
            template <class C>
            static std::true_type check(decltype(&C::to_string));
            template <class C>
            static std::false_type check(...);
        };
        template <class C>
        class has_to_string : public decltype(has_to_string_impl::check<C>(nullptr)) {};

        template <typename T>
        using is_direct_out = std::disjunction<
            is_string<T>
            ,is_string_view<T>
            >;

        template <typename T>
        using is_use_std_to_string = std::disjunction<
            std::is_integral<T>
            ,std::is_floating_point<T>
            >;
        /*! Dump variable (simple) .
         *
         *  With variable's type name.
         *
         *  Only dump value from int,float,double,std::string,std::string_view and C language formatted c* and other show only type name 
         *  \tparam T variable type
         *  \param[in] v dump target
         *  \retval string from variable value.
         */
        template <typename T>
        inline std::string dump_var_simple(T v)
        {
            if constexpr (is_string_v<T>) {
                return v;
            }
            else if constexpr (is_string_view_v<T>) {
                return std::string(v);
            }
            else if constexpr (is_use_std_to_string<T>::value) {
                return std::to_string(v);
            }
            else if constexpr (std::is_pointer_v<T>) {
                if constexpr (is_c_string<T>::value) {
                    return toReadableCtrlCode(v);
                }
                std::stringstream ss;
                ss << v;
                return dump_var_simple(*v) + " @" + ss.str();
            }
            else if constexpr (std::is_class_v<T>) {
                if constexpr (has_to_string<T>::value) {
                    return v.to_string();
                }
                std::string str{"class: "};
                str += demangle(typeid(decltype(v)).name());
                return str;
            }
            else {
                return std::string {"unknown"};
            }
            return std::string {"Mmmmmm-----"};
        } //<-- function dump_var_simple ends here.

        /*!  Dump variable into string object.
         *
         *  \tparam T is variable type
         *  \param[in] v is variable name from defined macro
         *  \param[in] t is variable
         *  \retval formated std::string
         */
        template <typename T>
        inline auto dump_str(const char* v, T t) -> std::string
        {
            std::string message{SML_SGR_BOLD};
            message += SML_SGR_CYAN;// << TEXT(v) << " => " << Sml::Debug::dump_var_simple(v) << SML_SGR_RESET << std::endl
            message += v;
            message += " => ";
            message += dump_var_simple(t);
            message += SML_SGR_RESET;
            return message;
        }
        /*!  Dump variable into out_message.
         *
         *  \tparam T is variavle type
         *  \param[in] v is variable name from defined macro
         *  \param[in] t is variable
         */
        template <typename T>
        inline auto dump(const char* v, T t) -> void
        {
            out_message(dump_str(v, t));
        }
        /*! Perfomance checker .
         *
         * 
         *  \tparam F perfomance check taget function
         *  \tparam Args taget function's argument(s)
         *  \param[in] rep number of loop iteration
         *  \param[in] count number of loop iteration for avarage
         *  \param[in] f target function
         *  \param[in] ...args target function's argument (if use)
         *  \retval sec in micro seconds
         */
        template <typename F, typename...Args>
        inline auto performance(size_t rep, size_t count, F f, Args...args)
        {
            using namespace std::chrono;
            using timer = high_resolution_clock;
            auto n = timer::now();
            auto x = duration_cast<nanoseconds>(n - n).count();
            for (decltype(rep) c = 0; c < count; ++c) {
                auto start = timer::now();
                for (decltype(rep) i = 0; i < rep; i++) {
                    f(args...);
                }
                auto stop = timer::now();
                x += duration_cast<nanoseconds>(stop - start).count();
            }
            auto diff = x / count;
            std::string message{SML_SGR_BOLD};
            std::clog << SML_SGR_BOLD << SML_SGR_RED
                      << "total : "
                      << diff
                      << " avg. "
                      << diff / rep
                      << " in [ns]"
                      << SML_SGR_RESET << std::endl;
            return diff;
        }
        class sml_assert : public std::exception {
        public:
            explicit sml_assert(std::string r) : m_reason(std::move(r)) {}
            virtual auto what() const noexcept(true) -> const char* override
            {
                return m_reason.c_str();
            }
        private:
            std::string m_reason;
        };
        /*! Occure abort or exception when call this function.
         *
         *  All parameter(s) will comes from macro named SmlASERT.
         *
         *  \param[in] cond Assertion condition code
         *  \param[in] extend extend message
         *  \param[in] noExcept flag true: abort when assertion occurred, false: throw sml_assert exception when assertion occurred
         *  \param[in] file name of assertion occurred file
         *  \param[in] function name of assertion occurred function
         *  \param[in] line number of assertion occurred line
         */
        inline auto assertion(const char* cond, const char* extend, bool noExcept, const std::string_view& file, const char* function, int line)
        {
            std::string message{SML_SGR_BOLD};
            message += SML_SGR_RED;
            message += "Sml assertion occurred: ";
            message += cond;
            message += "  [[";
            message += extend;
            message += "]]\n\t";
            message += SML_SGR_YELLOW;
            message += "function=> ";
            message += function;
            message += "\n\tin ";
            message += std::string(file.begin(), file.end());
            message += " at ";
            message += std::to_string(line);
            message += SML_SGR_RESET;
            if (noExcept) {
                out_message(message);
                abort();
            } else {
                throw sml_assert(message.c_str());
            }
        } //<-- function assertion ends here.
        inline auto assertion_ok(const char* cond, const std::string_view& file, const char* function, int line)
        {
# if defined (SML_ASSERT_CHECK) && (SML_ASSERT_OK)
            std::string message{SML_SGR_BOLD};
            message += SML_SGR_YELLOW;
            message += "PASS ";
            message += "  [[";
            message += cond;
            message += "]] ";
            message += "function=> ";
            message += function;
            message += " in ";
            message += std::string(file.begin(), file.end());
            message += " at ";
            message += std::to_string(line);
            message += SML_SGR_RESET;
            out_message(message);
# else
            SML_UNUSED_ARG(cond); SML_UNUSED_ARG(file); SML_UNUSED_ARG(function); SML_UNUSED_ARG(line);
#  endif
        }
        /*! Show condition.
         *
         *  All parameter(s) will comes from macro named SML_CHECK.
         *
         *  \param[in] cond Assertion condition code
         *  \param[in] file name of assertion occurred file
         *  \param[in] function name of assertion occurred function
         *  \param[in] line number of assertion occurred line
         */
        inline auto check(const char* cond, const std::string_view& file, const char* function, int line)
        {
            std::string message{SML_SGR_BOLD};
            message += SML_SGR_RED;
            message += "Sml assertion occurred: ";
            message += cond;
            message += "\n\t";
            message += SML_SGR_CYAN;
            message += "function=> ";
            message += function;
            message += "in ";
            message += std::string(file.begin(), file.end());
            message += " at ";
            message += std::to_string(line);
            message += SML_SGR_RESET;
            out_message(message);
            return false;
        } //<-- function check ends here.
        inline auto check_ok(const char* cond, const std::string_view& file, const char* function, int line)
        {
# if defined (SML_ASSERT_CHECK) && (SML_ASSERT_OK)
            std::string message{SML_SGR_BOLD};
            message += SML_SGR_CYAN;
            message += "PASS ";
            message += "  [[";
            message += cond;
            message += "]] ";
            message += "function=> ";
            message += function;
            message += " in ";
            message += std::string(file.begin(), file.end());
            message += " at ";
            message += std::to_string(line);
            message += SML_SGR_RESET;
            out_message(message);
# else
            SML_UNUSED_ARG(cond); SML_UNUSED_ARG(file); SML_UNUSED_ARG(function); SML_UNUSED_ARG(line);
#  endif
        } //<-- function check_ok ends here.

        /*! Trace .
         *
         *  All parameter(s) will comes from macro named TRACE when set SML_TRACE.
         *
         */
        inline auto trace(const char* msg, const char* name, int line) -> const std::string
        {
            std::string message{SML_SGR_BOLD};
            message += SML_SGR_GREEN;
            message += "====> ";
            message += msg;
            message += "\n function is ";
            message +=  SML_SGR_CYAN;
            message += name;
            message +=  SML_SGR_RESET;
            message += " ";
            message += std::to_string(line);
            return message;
        }
        /*!  .
         *
         *
         */
        inline auto message(const char* msg, int line) -> const std::string
        {
            std::string mesg{SML_SGR_BOLD};
            mesg += SML_SGR_GREEN;
            mesg += "===> ";
            mesg += msg;
            mesg += "===> ";
            mesg += std::to_string(line);
            mesg += SML_SGR_RESET;
            return mesg;
        }
        static constexpr std::array<char[12],8> LogLevelNameList = {
            " [Fatal ]: ",
            " [Error ]: ",
            " [Warn  ]: ",
            " [Notice]: ",
            " [Info  ]: ",
            " [Debug ]: ",
        }; //!< Logging level name list 0->fatal to 5->debug
        /*! Build log message.
         *
         * fname and line number comes from preprocessor embedded macro
         */
        inline auto build_log_message(const std::string& body, const std::string_view& fname, int line, int lv = 5) -> const std::string
        {
            auto  color = SML_SGR_RED;
            switch (lv) {
            case 0:                                        // fatal red
            case 1: break;                                 // Error red
            case 2:                                        // Warn yellow
            case 3: color = SML_SGR_YELLOW; break;        // Notice yellow
            case 4:                                        // Info cyan
            case 5: color = SML_SGR_CYAN; break;          // debug cyan
            default: lv = 5; color = SML_SGR_CYAN; break; //
            }
            std::stringstream ss;
            ss << SML_SGR_BOLD
               << color
               << LogLevelNameList[lv]
               << convertTime_tToStr(getElapsedTime())
               << " ("
               << std::this_thread::get_id()
               << ") "
               << body
               << " in "
               << fname
               << " at "
               << std::to_string(line)
               << SML_SGR_RESET;
            return ss.str();
        }
        inline auto message(const std::string& body, const std::string_view& fname, int line)
        {
            std::clog << SML_SGR_BOLD << SML_SGR_BLUE
                      << convertTime_tToStr(getElapsedTime())
                      << " ["
                      << std::this_thread::get_id()
                      << "] "
                      << body
                      << " in "
                      << fname
                      << " at "
                      << std::to_string(line)
                      << SML_SGR_RESET
                      << std::endl;
        }
        /*! Marker .
         *
         *  All parameter(s) will comes from macro named MARK when set SML_TRACE.
         *
         */
        inline  auto mark(const std::string_view& file, int line)
        {
            std::stringstream ss;
            ss << SML_SGR_BOLD
               << getCurrentTime()
               << " ***** mark ====> "
               << " in "
               << file
               << " at "
               << std::to_string(line)
               << " <==== marK *****";
            return ss.str();
        }
        /*!  Calculate address gap between head to tail.
         *
         *  \tparam T is type of pointer
         *  \param[in] head pointer (top address)
         *  \param[in] tail pointer (botom address)
         *  \param[in] hvar is name of head pointer (mybe create the defined macro)
         *  \param[in] tvar is name of tail pointer (mybe create the defined macro)
         *  \param[in] disp is switch to show display default 'show' when false 'not show'
         */
        template <typename T>
        inline auto size_gap(T head, T tail, const char* hvar, const char* tvar, bool disp = true) -> const std::string
        {
            decltype(head) h = nullptr;
            decltype(head) t = nullptr;
            size_t sz = 0;
            if constexpr (std::is_pointer_v<T>) {
                h = head;
                t = tail;
                sz = sizeof(std::remove_pointer_t<T>);
            } else {
                h = std::addressof(head);
                t = std::addressof(tail);
                sz = sizeof(T);
            }
            auto gap = t - h;
            auto gap_byte = reinterpret_cast<char*>(t) - reinterpret_cast<char*>(h);
            std::stringstream os;
            os << SML_SGR_BOLD
               << SML_SGR_GREEN
               << "====> "
               << tvar
               << " to "
               << hvar
               << " gap is "
               << gap_byte
               << " byte / "
               << gap
               << " objects "
               << SML_SGR_CYAN
               << " @object size is (including padding) "
               << sz
               << SML_SGR_GREEN
               << " <===="
               << SML_SGR_RESET;
            if (disp)
                std::clog << os.str() << std::endl;
            return os.str();
        }
        static constexpr std::array<char[6], 8 * 20> ctrlCharTbl = {
            "[NUL]","[SOH]","[STX]","[ETX]","[EOT]","[ENQ]","[ACK]","[BEL]",
            // "[NUL]","[01H]","[STX]","[ETX]","[04H]","[05H]","[ACK]","[07H]", //  1
            "[ BS]","[ HT]","[ LF]","[ VT]","[ FF]","[ CR]","[ SO]","[ SI]",
            // "[08H]","[09H]","[0aH]","[0bH]","[0cH]","[0dH]","[0eH]","[0fH]", //  2
            "[DEL]","[DC1]","[DC2]","[DC3]","[DC4]","[NAK]","[SYN]","[ETB]",
            // "[10H]","[11H]","[12H]","[13H]","[14H]","[NAK]","[16H]","[17H]", //  3
            "[CAN]","[ EM]","[SUB]","[ESC]","[ FS]","[ GS]","[ RS]","[ US]",
            // "[18H]","[19H]","[1aH]","[1bH]","[1cH]","[1dH]","[1eH]","[1fH]", //  4

            "[80H]","[81H]","[82H]","[83H]","[84H]","[85H]","[86H]","[87H]", //  5
            "[88H]","[89H]","[8aH]","[8bH]","[8cH]","[8dH]","[8eH]","[8fH]", //  6
            "[90H]","[91H]","[92H]","[93H]","[94H]","[95H]","[96H]","[97H]", //  7
            "[98H]","[99H]","[9aH]","[9bH]","[9cH]","[9dH]","[9eH]","[9fH]", //  8
            "[a0H]","[a1H]","[a2H]","[a3H]","[a4H]","[a5H]","[a6H]","[a7H]", //  9
            "[a8H]","[a9H]","[aaH]","[abH]","[acH]","[adH]","[aeH]","[afH]", // 10
            "[b0H]","[b1H]","[b2H]","[b3H]","[b4H]","[b5H]","[b6H]","[b7H]", // 11
            "[b8H]","[b9H]","[baH]","[bbH]","[bcH]","[bdH]","[beH]","[bfH]", // 12
            "[c0H]","[c1H]","[c2H]","[c3H]","[c4H]","[c5H]","[c6H]","[c7H]", // 13
            "[c8H]","[c9H]","[caH]","[cbH]","[ccH]","[cdH]","[ceH]","[cfH]", // 14
            "[d0H]","[d1H]","[d2H]","[d3H]","[d4H]","[d5H]","[d6H]","[d7H]", // 15
            "[d8H]","[d9H]","[daH]","[dbH]","[dcH]","[ddH]","[deH]","[dfH]", // 16
            "[e0H]","[e1H]","[e2H]","[e3H]","[e4H]","[e5H]","[e6H]","[e7H]", // 17
            "[e8H]","[e9H]","[eaH]","[ebH]","[ecH]","[edH]","[eeH]","[efH]", // 18
            "[f0H]","[f1H]","[f2H]","[f3H]","[f4H]","[f5H]","[f6H]","[f7H]", // 19
            "[f8H]","[f9H]","[faH]","[fbH]","[fcH]","[fdH]","[feH]","[EOF]", // 20
        }; //!<  Control character code table (It is necessary to map the outside of the readable range of the character table to 0x20 after code 0x80)
        /*! Convert control characters to readable strings
         *
         *  \param[in] s Source string of convert
         *  \retval converted characters(string)
         */
        inline std::string toReadableCtrlCode(const std::string& s)
        {
            std::string d{};
            std::string t;
            for (size_t i = 0; i < s.size(); ++i) {
                auto x = (static_cast<size_t>(s[i])) & 0xff;
                if (x < 0x20) {
                    d += ctrlCharTbl[x];
                } else if (x == 0x20) {
                    d.append("[SPC]");
                } else if (x == 0x7f) {
                    d.append("[DEL]");
                } else if (x > 0x7f) {
                    if (x == 0xff) {
                        d.append("[EOF]");
                    } else {
                        d.append(ctrlCharTbl[static_cast<std::uint8_t>(x - 0x60)]);
                    }
                } else {
                    d.push_back(x);
                }
            }
            return d;
        } //<-- function toReadableCtrlCode ends here.

        static constexpr std::array<char[3], 16*16> hexChar256Tbl = {
            "00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f",
            "10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f",
            "20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f",
            "30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f",
            "40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f",
            "50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f",
            "60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f",
            "70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f",
            "80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f",
            "90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f",
            "a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af",
            "b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf",
            "c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf",
            "d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df",
            "e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef",
            "f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff",
        };
        /*! Convert a Byte sequence to a hexadecimal representation string
         *
         *  \param[in] s Source string of convert
         *  \retval converted characters(string)
         */
        inline std::string hexDump(const std::string& s)
        {
            std::string t{};
            for (auto d : s) {
                t.append(hexChar256Tbl[(static_cast<std::uint8_t>(d)) & 0xff]);
            }
            return t;
        } //<--  function hexDump ends here.
    } //<-- namespace Debug ends here.


    /*! DEBUG exaples
     *
     * \example debug/example.cpp
     */
    
} //<-- namespace Sml ends here.


#endif //<-- macro  SML_DEBUG_Hpp ends here.
