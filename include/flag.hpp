/*!
 * \file flag.hpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Flag class with mutex lock
 *
 * \author s3mat3
 */

#pragma once

#ifndef FLAG_Hpp
# define  FLAG_Hpp

# include <mutex>
# include <concepts>
# include <type_traits>


namespace Sml {
    template <typename T>
    struct is_bool {static constexpr bool value = false;};

    template <>
    struct is_bool<bool> {static constexpr bool value = true;};

    template<typename T>
    inline constexpr bool is_bool_v = is_bool<T>::value;

    template <typename T>
    concept flag_concept = std::disjunction_v<
        is_bool<T>
        , std::is_integral<T>
        , std::is_floating_point<T>
        >; //!< flag concept is only boolean, integer or floating point
    template <typename T>
    concept bool_type_concept = is_bool_v<T>;

    template <flag_concept T>
    class Flag
    {
    public:
        using lock_type  = std::mutex;
        using guard_type = std::lock_guard<lock_type>;

        constexpr explicit Flag(T initial) : m_value{initial}, m_lock {} {}

        Flag()                           = default;
        ~Flag()                          = default;
        Flag(const Flag&)                = default;
        Flag(Flag&&) noexcept            = default;
        Flag& operator=(const Flag&)     = default;
        Flag& operator=(Flag&&) noexcept = default;

        auto update(T value)
        {
            guard_type lock(m_lock);
            m_value = value;
        }

        auto value() const noexcept
        {
            guard_type lock(m_lock);
            return m_value;
        }

        auto operator=(T rhs) noexcept {update(rhs);}
        auto operator()() const noexcept {return value();}

        operator bool() const noexcept
        {
            if constexpr (is_bool_v<T>) {
                return value();
            } else {
                return false;
            }
        }
    private:
        T                 m_value {};
        mutable lock_type m_lock  {};
    };

    using flag_t = Flag<bool>;
    /*! like a register flag .
     */
    class FlagRegister
    {
    public:
        using guard            = std::lock_guard<std::mutex>;
        using value_type       = std::uint64_t;
        using bit_pattern_type = value_type;

        static constexpr bit_pattern_type zero = 0;
        static constexpr bit_pattern_type one  = 1;
        static constexpr bit_pattern_type lsb  = one;
        static constexpr bit_pattern_type msb  = 0x8000'0000'0000'0000;
        /*! Generate const flag value from one by shifted left to position bit(s).
         */
        static constexpr bit_pattern_type shl(size_t position) {return FlagRegister::one << position;}
        /*! Create mask pattern .
         */
        static constexpr bit_pattern_type mask(bit_pattern_type target) {return ~(target);}

        constexpr FlagRegister() = default;
        constexpr explicit FlagRegister(bit_pattern_type b) : m_register {b}, m_guard {} {}
        FlagRegister(const FlagRegister&) = delete; //!< mutex is not copyable
        FlagRegister(FlagRegister&&) noexcept = delete; //!< mutex is not movable
        ~FlagRegister() = default;
        /*! Check for bit pattern .
         */
        auto is_set(bit_pattern_type check_position) const noexcept -> bool {guard lock(m_guard); return (m_register & check_position);}
        /*! Set bit pattern .
         */
        auto set(bit_pattern_type bit_pattern)   noexcept -> void {guard lock(m_guard); m_register |= bit_pattern;}
        /*! Reset bit pattern .
         */
        auto reset(bit_pattern_type bit_pattern) noexcept -> void {guard lock(m_guard); m_register &= FlagRegister::mask(bit_pattern);}
        /*! Set after reset .
         */
        auto set_reset(bit_pattern_type set_pattern, bit_pattern_type reset_pattern) noexcept -> void
        {
            guard lock(m_guard);
            m_register |= set_pattern;
            m_register &= FlagRegister::mask(reset_pattern);
        }
        /*! Get value .
         */
        auto value() const noexcept -> value_type {guard lock(m_guard); return m_register;}
        /*! Cler value by ZERO .
         */
        auto clear() {guard lock(m_guard); m_register = FlagRegister::zero;}
    private:
        value_type         m_register {FlagRegister::zero}; //!< holder
        mutable std::mutex m_guard    {};                   //!< locker

    }; //<-- class FlagRegister ends here.
} //<-- namespace Sml ends here.
#endif //<-- macro  FLAG_Hpp ends here.
