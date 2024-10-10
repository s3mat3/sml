/*!
 * \addtogroup io
 * @{
 * \file channel.hpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#pragma once

#ifndef CHANNEL_Hpp
# define  CHANNEL_Hpp

# include <csignal>
# include <sys/select.h>

# include "base.hpp"
# include "io/io.hpp"
# include "io/status_flag.hpp"

namespace Sml {
    namespace IO {
        /*! Device connection parameter(s) abstract base class .
         */
        class ConnectionParameterBase : public Sml::Base
        {
        public:
            using timespec_type = std::timespec;
            using timespec_ptr  = std::shared_ptr<timespec_type>;
            using sigset_ptr    = std::shared_ptr<sigset_t>;
            using Sml::Base::Base;

            explicit ConnectionParameterBase(const std::string& name) : Base(name) {}
            ConnectionParameterBase() : ConnectionParameterBase(""s) {}
            ConnectionParameterBase(const ConnectionParameterBase&) = delete;
            ConnectionParameterBase(ConnectionParameterBase&&) noexcept = delete;

            virtual ~ConnectionParameterBase() = default;
        };
        /*!  abstract class ChannelBase.
         * for communication channel base class
         */
        class ChannelBase
        {
        public:
            using timespec_type = std::timespec;
            using timespec_ptr  = std::shared_ptr<timespec_type>;
            using sigset_ptr    = std::shared_ptr<sigset_t>;
            using byte_buffer   = std::string;
            using status_flag   = StatusFlag;

            ChannelBase()
                : m_timeout{std::make_unique<timespec_type>(0, 0)}
                , m_mask{std::make_unique<sigset_t>()}
            {
                ::sigemptyset(m_mask.get());
                ::sigaddset(m_mask.get(), SIGUSR1);
                ::sigaddset(m_mask.get(), SIGUSR2);
                ::sigaddset(m_mask.get(), SIGPIPE);
                ::sigaddset(m_mask.get(), SIGALRM);
            }
            ChannelBase(const ChannelBase&) = delete;
            ChannelBase(ChannelBase&&) noexcept = delete;
            virtual ~ChannelBase() = default;
            /*! setup by derived class .
             */
            virtual auto setup() noexcept -> void {}
            /*! connect .
             */
            virtual auto connect() noexcept -> return_code {return IO_NOT_OPEN;}
            /*! disconnect .
             */
            virtual auto disconnect() noexcept -> return_code {return IO_NOT_OPEN;}
            /*! Read IO .
             *  \param[out] readed mean readed data from channel
             */
            virtual auto read(byte_buffer& readed) noexcept -> return_code = 0;
            /*! Write IO .
             *  \param[in] forSend mean sending data to channel
             */
            virtual auto write(const byte_buffer& forSend) noexcept -> return_code = 0;
            auto status() const noexcept -> const status_flag& {return m_status;}
            auto status() noexcept -> status_flag& {return m_status;}
            /*! Check IO ready .
             * This function is impremented by default, using pselect nonblocking mode.
             *  \param[in] d is direction code direction::in wait readable, direction::out wait writeable
             */
            virtual auto isReady(direction d) noexcept -> return_code
            {
                auto s = status_flag::failure;
                ssize_t ret = -1;
                fd_set fdset;
                FD_ZERO(&fdset);
                FD_SET(m_fd, &fdset);

                if (d == direction::in) {
                    s = status_flag::ready_read;
                    ret = pselect(m_fd + 1, &fdset, NULL, NULL, m_timeout.get(), m_mask.get());
                } else if (d == direction::out) {
                    s = status_flag::ready_write;
                    ret = pselect(m_fd + 1, NULL, &fdset, NULL, m_timeout.get(), m_mask.get());
                }
                errno_t num = errno; // set error number
                // update flag
                if (ret > 0) { // PSELECT
                    if (FD_ISSET(m_fd, &fdset)) {
                        FD_CLR(m_fd, &fdset);
                        m_status.set_reset(s, status_flag::timeouted); //
                        return IO_OK;
                    } else {
                        m_status.set_reset(status_flag::timeouted, s);
                        SML_FATAL("=====> What happen!!");
                        ret = IO_TIMEOUT; // ret code reset for timeout
                    }
                } else if (ret == 0) { // no hit but no error
                    m_status.set_reset(status_flag::timeouted, s);
                    return IO_TIMEOUT;
                } else { // ret < 0
                    if (num == EAGAIN) { // detect EAGAIN it's not error because occure time out or non block and not ready other side data
                        m_status.set_reset(status_flag::timeouted, s);
                        return IO_TIMEOUT; // ret code reset for timeout
                    } else {
                        m_status.set_reset(status_flag::failure, s);
                        SML_FATAL("=====> in fd "s + std::to_string(m_fd) + " error code > "s + std::to_string(num));
                        return IO_FAILURE;
                    }
                } //<-- PSELECT result check ends here.
                return static_cast<return_code>(ret);
            }
        protected:
            fd_type      m_fd      {void_fd()}; //!< communication channel target fd
            status_flag  m_status  {};          //!< communication channel status
            timespec_ptr m_timeout {nullptr};   //!< for use pselect system call
            sigset_ptr m_mask{nullptr};         //!< for use pselect system call
        }; //<-- abstract class ChannelBase ends here.
        }  // namespace IO
        }  // namespace Sml

#endif //<-- macro  CHANNEL_Hpp ends here.
/** @} */
