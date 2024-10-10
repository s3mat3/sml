/*!
 * \addtogroup ds
 * @{
 * \file signal.hpp
 *
 * \copyright © 2024 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Signal sender/reciver in blocked
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_SIGNAL_Hpp
# define  SML_SIGNAL_Hpp

# include <chrono>
# include <condition_variable>
# include <exception>
# include <mutex>

# include "sml.hpp"

namespace Sml {
    /*! Exception for signal wait cancel.
     *
     *
     */
    struct canceled_wait_event : std::exception {};

    /*! Signal .
     *
     * Signal delivery with blocking.
     * Usecase is one writer and one reader.
     * Don't more reader, because no queue.
     */
    class Signal {
        using signal_id = return_code;
        using lock_type = std::mutex;
        using locker    = std::unique_lock<lock_type>;
        using cv        = std::condition_variable;
        static constexpr signal_id TIMEOUT = Sml::TIMEOUT;
    public:
        /*! Constructor 1 .
         *
         * necessary default constructable SignalCode as m_id
         */
        Signal()                             = default;
        ~Signal()                            = default;

        Signal(const Signal&)                = delete;
        Signal(Signal&&) noexcept            = delete;
        Signal& operator=(const Signal&)     = delete;
        Signal& operator=(Signal&&) noexcept = delete;
        /*!  Update new signal code.
         */
        void update(const signal_id& x)
        {
            locker guard(m_guard);
            m_updated = true;
            m_id = x;
            m_monitor.notify_all();
        }
        /*! Wait update .
         */
        signal_id wait_update() noexcept(false)
        {
            locker guard(m_guard);
            m_monitor.wait(guard, [&] {return m_updated || m_canceled;});
            if (m_canceled) {
                m_canceled = false;
                throw canceled_wait_event();
            }
            if (m_updated) m_updated = false;
            return m_id;
        }
        /*! Wait update with timeout.
         */
        signal_id wait_for(millisec_interval tout) noexcept(false)
        {
            locker guard(m_guard);
            auto ret = m_monitor.wait_for(guard
                                       , std::chrono::milliseconds(tout)
                                       , [&] {return m_updated || m_canceled;});
            if (m_canceled) {
                m_canceled = false;
                throw canceled_wait_event();
            }
            if (m_updated) m_updated = false;
            if (! ret) return TIMEOUT;
            return m_id;
        }
        void clear()
        {
            locker guard(m_guard);
            m_updated  = false;
            m_canceled = false;
        }
        /*! cancel for wait .
         */
        void cancel()
        {
            locker guard(m_guard);
            m_canceled = true;
            m_monitor.notify_all();
        }
    private:
        bool      m_updated  {false}; //!< updated flag true: update occurred, false: no update
        bool      m_canceled {false}; //!< cancel flag true: cancel occurrerd, false: no action
        signal_id m_id       {0};     //!< signal id
        lock_type m_guard    {};      //!< resource guard
        cv        m_monitor  {};      //!< monitor with condition variable
    }; //<-- class Signal ends here.

    /*! \class Signal
     * \example signal/example.cpp
     */
    } // namespace Sml

#endif //<-- macro  SML_SIGNAL_Hpp ends here.
/** @} */
