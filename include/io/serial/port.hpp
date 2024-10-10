/*!
 * \addtogroup io
 * @{
 *   \defgroup com IO/serial-port
 *   @{
 * \file port.hpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Serial port implement (Only posix)
 *
 * \author s3mat3
 */

#pragma once

#ifndef PORT_Hpp
# define  PORT_Hpp

# include <sys/ioctl.h>
# include "params.hpp"

namespace Sml {
    namespace IO {
        namespace Serial {
            /*! Serial port .
             */
            class Port : public ChannelBase
            {
            public:
                using byte_buffer_t = ByteArray_t;
                using params_ptr = std::unique_ptr<Parameters>;
                explicit Port(const std::string& name, ConnectionConditions p)
                    : ChannelBase {}
                    , m_param {std::make_unique<Parameters>(name, p)}
                {}
                Port(const std::string& name, Baudrate b, Parity p, FlowControl f, StopBit s, CharSize w)
                    : ChannelBase {}
                    , m_param {std::make_unique<Parameters>(name, b, p, f, s, w)}
                {}
                Port(const std::string& name, Baudrate b, Parity p, FlowControl f, StopBit s)
                    :Port(name, b, p, f, s, CharSize::W8) {}
                Port(const std::string& name, Baudrate b, Parity p, FlowControl f)
                    :Port(name, b, p, f, StopBit::ONE, CharSize::W8) {}
                Port(const std::string& name, Baudrate b, Parity p)
                    :Port(name, b, p, FlowControl::NONE, StopBit::ONE, CharSize::W8) {}
                explicit Port(const std::string& name, Baudrate b)
                    :Port(name, b, Parity::NONE, FlowControl::NONE, StopBit::ONE, CharSize::W8) {}
                explicit Port(const std::string& name)
                    :Port(name, Baudrate::BPS115200, Parity::NONE, FlowControl::NONE, StopBit::ONE, CharSize::W8) {}
                Port()
                    :Port("/dev/ttyS0"s, Baudrate::BPS115200, Parity::NONE, FlowControl::NONE, StopBit::ONE, CharSize::W8) {}
                virtual ~Port()
                {
                    disconnect();
                }
                /*!  Setup serial port by params.
                 */
                auto setup() noexcept -> void override
                {
                }
                /*!  Connect to device (Open , FD setup and swap termios new/save).
                 */
                auto connect() noexcept -> return_code override
                {
                    if (m_status.is_set(status_flag::opened)) {
                        this->disconnect();
                    }
                    m_fd = ::open(m_param->name().c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
                    if (m_fd < 3) {
                        m_status.set_reset(status_flag::failure, status_flag::opened);
                        return FAILURE;
                    }
                    m_param->setup(m_fd);
                    m_status.reset_error();
                    m_status.set(status_flag::opened);
                    return OK;
                }
                /*!  Disconnect (close and restore termios).
                 */
                auto disconnect() noexcept -> return_code override
                {
                    if (m_fd > 2) { //
                        m_status.clear();
                        m_param->ioctl()->restoreTermios(m_fd);
                        auto x =static_cast<return_code>(::close(m_fd));
                        m_fd = void_fd();
                        return x;
                    }
                    return FAILURE;
                }
                /*! Write data .
                 */
                auto write(const byte_buffer_t& buff) noexcept -> return_code override
                {
                    auto ret = ::write(m_fd, buff.data(), buff.size());
                    return static_cast<return_code>(ret);
                }
                /*! Read data .
                 */
                auto read(byte_buffer_t& buff) noexcept -> return_code override
                {
                    auto ret = ::read(m_fd, buff.data(), buff.capacity());
                    //if (ret >= 0) buff.update_tail(static_cast<size_type>(ret));
                    return static_cast<return_code>(ret);
                }
                // under hardware manipulator
                auto rts(bool state) const noexcept
                {
                    int flg = TIOCM_RTS;
                    (state) ? ::ioctl(m_fd, TIOCMBIS, &flg) : ::ioctl(m_fd, TIOCMBIC, &flg);
                    return state;
                }
                auto rts() const noexcept
                {
                    int flg {0};
                    ::ioctl(m_fd, TIOCMGET, &flg);
                    return (flg & TIOCM_RTS) ? true : false;
                }
                auto cts() const noexcept
                {
                    int flg {0};
                    ::ioctl(m_fd, TIOCMGET, &flg);
                    return (flg & TIOCM_CTS) ? true : false;
                }
                auto dtr(bool state) const noexcept
                {
                    int flg = TIOCM_DTR;
                    (state) ? ::ioctl(m_fd, TIOCMBIS, &flg) : ::ioctl(m_fd, TIOCMBIC, &flg);
                    return state;
                }
                auto dtr() const noexcept
                {
                    int flg {0};
                    ::ioctl(m_fd, TIOCMGET, &flg);
                    return (flg & TIOCM_DTR) ? true : false;
                }
                auto dsr() const noexcept
                {
                  int flg{0};
                  ::ioctl(m_fd, TIOCMGET, &flg);
                  return (flg & TIOCM_DSR) ? true : false;
                }

              protected:
                params_ptr m_param;
            }; //<-- class Port ends here.
        }  // namespace Serial
    }  // namespace IO
}  // namespace Sml

#endif //<-- macro  PORT_Hpp ends here.
/**   @}
 *  @}
 */
