/*!
 * \file result.hpp
 *
 * \copyright © 2024 s3mat3
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Result type for function
 *
 * \author s3mat3
 */
#pragma once

#ifndef SML_RESULT_Hpp
# define  SML_RESULT_Hpp

// # include <type_traits>
# include "sml.hpp"
# if 0
# define SML_TRACE 1
#endif
# include "debug.hpp"

namespace Sml {
    class bad_result_access : public std::exception {
    public:
        explicit bad_result_access(std::string r) : m_reason(std::move(r)) {}
        virtual auto what() const noexcept(true) -> const char* override
        {
            return m_reason.c_str();
        }
    private:
        std::string m_reason;
    };
    /*! Error code holder .
     *
     * This class's purpose is distinction return_code and integer 
     * for Result class template parameter
     */
    struct error_type final
    {
        constexpr error_type() = default;
        constexpr explicit error_type(return_code e) : m_error(e) {}
        constexpr explicit error_type(const error_type&) = default;
        constexpr explicit error_type(error_type&& rhs) noexcept {if (this != &rhs) m_error = rhs.m_error;}
        constexpr error_type& operator=(const error_type& rhs) = default;
        constexpr error_type& operator=(const error_type&& rhs) noexcept {if (this != &rhs) {m_error = rhs.m_error;} return *this;}
        ~error_type() = default;
        constexpr auto code() const noexcept ->return_code {return m_error;}
        constexpr auto operator()() const noexcept -> return_code {return m_error;}
        return_code m_error {-1}; //!< error code(return code)
    }; //<-- struct error_type ends here.

    /*! Result type checker .
     *
     *  \tparam T is result value type
     */
    template <typename T>
    using is_result_requirement = std::conjunction<
        std::is_copy_constructible<T>
        , std::is_move_constructible<T>
        , std::negation<std::is_reference<T>>
        , std::negation<std::is_pointer<T>>
        , std::negation<std::is_same<error_type, T>>
        >;
    /*! Result class .
     *
     * This class can propagate value_type or error_type to caller as appropriate.
     *
     *  \tparam T value_type has constraint, that is movable, copyable, not error_type, not pointer and not reference.
     */
    template <typename T>
    class Result
    {
        static_assert(is_result_requirement<T>::value, "Necessary the typename T can be copy and move constructible and Not pointer and reference");
        using value_type = T;
        using reference = Result<value_type>&;
        using const_reference = const Result<value_type>&;
        using rvalue_reference = Result<value_type>&&;
    public:
        /*! constructor 1 .
         *
         * No effect...
         */
        constexpr Result() = default;
        /*! constructor 2 .
         *
         * for create result value from value_type reference
         */
        constexpr explicit Result(const value_type& v) : m_has_value(true) {construct_value(v);}
        /*! constructor 3 .
         *
         * for create result value from value_type rvalue reference
         */
        constexpr explicit Result(value_type&& v) : m_has_value(true) {construct_value(v);}
        /*! constructor 4 .
         *
         * for create result value with the result object constructing
         */
        template <class... Args> constexpr Result(Args... args) : m_has_value(true) {construct_value(args ...);}
        // error_type result
        /*! constructor 5 .
         *
         * for create error value from error_type reference
         */
        constexpr explicit Result(const error_type& e) : m_has_value(false) {construct_error(e);}
        /*! constructor 6 .
         *
         * for create error value from error_type rvalue reference
         */
        constexpr explicit Result(error_type&& e) : m_has_value(false) {construct_error(e);}
        // /*! constructor 7 .
        //  *
        //  * for create result value with the result object constructing
        //  */
        // template < class Arg > constexpr Result(Arg arg) : m_has_value(false) {construct_error(arg);}
        // copy move delete
        //Result(const_reference) = delete;
        //Result(rvalue_reference) = delete;
        // reference operator=(const_reference) = delete;
        // reference operator=(rvalue_reference) = delete;
        ~Result()
        {
            if constexpr (! std::is_trivially_destructible_v<value_type>) {
                // if !m_has_value maybe not constructed
                if (m_has_value) m_value.~T(); // call destructor
            }
            // error_type class allways trivially desutructible
            static_assert(std::is_trivially_destructible_v<error_type>, "All way true!!");
        }
        // accssessors
        /*!  resut value getter.
         *
         * if has_value is true
         *  \retval value in value_type (T)
         */
        constexpr auto value() const noexcept -> value_type {return this->m_value;}
        /*! error Result getter .
         *
         * if !has_value
         *  \retval error_type
         * if you need value, use error_type::code() function
         */
        constexpr auto error() const noexcept {return this->m_error();}
        auto error(error_type&& e) noexcept {construct_error(e);}
        /*! Check for existence of value.
         *
         *  \retval ture value existence
         *  \retval false error existence
         */
        constexpr auto has_value() const noexcept -> bool {return m_has_value;}
        /*!  Check for existance of value.
         *
         *
         */
        constexpr operator bool() const noexcept {return m_has_value;}
    private:
        //////////////////////////////////////////////
        // helper value_type / error_type constructors
        //////////////////////////////////////////////
        // value
        /// direct construct
        template <class... Args>
        constexpr auto construct_value(Args&&... args) noexcept -> void {new (std::addressof(m_value)) T(std::forward<Args>(args)...);}
        /// copy construct
        constexpr auto construct_value(const T& v) noexcept -> void {new (std::addressof(m_value)) T(v);}
        /// move construct
        constexpr auto construct_value(T&& v) noexcept -> void {new (std::addressof(m_value)) T(std::move(v));}
        /// error
        // direct construct
        template <class Arg>
        constexpr auto construct_error(Arg&& arg) noexcept -> void {new (std::addressof(m_error)) error_type(std::forward<Arg>(arg));}
        /// copy construct
        constexpr auto construct_error(const error_type& v) noexcept -> void {new (std::addressof(m_error)) error_type(v);}
        /// move construct
        constexpr auto construct_error(error_type&& v) noexcept -> void {new (std::addressof(m_error)) error_type(std::move(v));}
    private:
        bool m_has_value       = false;
        union {
            bool         dummy = false;
            value_type m_value;
            error_type m_error;
        };
    }; //<-- class Result ends here.

    /*!
     * \example result/example.cpp
     */

} //<-- namespace Sml ends here.

#endif //<-- macro  SML_RESULT_Hpp ends here.
