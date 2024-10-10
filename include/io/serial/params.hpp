/*!
 * \addtogroup io
 * @{
 *   \addtogroup com
 *   @{
 * \file params.hpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Serial port parameter
 *
 * \author s3mat3
 */

#pragma once

#ifndef PARAMS_Hpp
# define  PARAMS_Hpp

# include <fcntl.h>
# include <termios.h>
# include <unistd.h>
# include <initializer_list>

# include "io/channel.hpp"
/*! \def SerialPort_initializer
 * defined serial port connection parameter(s)
 */
# define SerialPort_initializer                                 \
    {                                                           \
        .m_baudrate   = Sml::IO::Serial::Baudrate::BPS115200,  \
        .m_data       = Sml::IO::Serial::CharSize::W8,         \
        .m_parity     = Sml::IO::Serial::Parity::NONE,         \
        .m_stop       = Sml::IO::Serial::StopBit::ONE,         \
        .m_flow       = Sml::IO::Serial::FlowControl::NONE,    \
        .m_xOnChar    = 0x11,                                   \
        .m_xOffChar   = 0x13,                                   \
        .m_useEOF     = false,                                  \
        .m_eof        = 0xff,                                   \
        .m_bufferSize = 256,                                    \
        .m_timeout    = 10,                                     \
    }

namespace Sml {
    namespace IO {
        namespace Serial {
            using return_code = IO::return_code;
            using fd_type = IO::fd_type;
            /*! Baudrate with speed_t (speed_t is defined in termios.h) .
             */
            enum class Baudrate : speed_t {
                BPS4800   = B4800,   //!< 4800 bps 
                BPS9600   = B9600,   //!< 9600 bps
                BPS19200  = B19200,  //!< 19200 bps
                BPS38400  = B38400,  //!< 38400 bps
                BPS57600  = B57600,  //!< 57600 bps
                BPS115200 = B115200, //!< 115200 bps
                BPS230400 = B230400, //!< 230200 bps
            };
            /*! Converter for Barate to speed_t .
             */
            inline constexpr auto baudrate(Baudrate b) {return static_cast<speed_t>(b);}
            /*! Charactor size for 1 frame data .
             */
            enum class CharSize : tcflag_t {
                W5 = CS5, 
                W6 = CS6,
                W7 = CS7,
                W8 = CS8,
            };
            /*! Converter for Charsize to tcflag_t .
             */
            inline constexpr auto char_size(CharSize w) {return static_cast<tcflag_t>(w);}
            enum class Parity {
                NONE,
                ODD,
                EVEN
            };
            enum class StopBit {
                ONE,
                TWO,
            };
            enum class FlowControl {
                NONE,
                XON_XOFF,
                RTS_CTS
            };
            /*! ConnectionConditions .
             * All members public for static initializer
             */
            struct ConnectionConditions
            {
            // public: // All member public for static initializer
                Baudrate    m_baudrate   {Baudrate::BPS115200};    //!< baudrate
                CharSize    m_width      {CharSize::W8};       //!< data bit width
                Parity      m_parity     {Parity::NONE};  //!< parity type
                StopBit     m_stop       {StopBit::ONE};     //!< stop bit size
                FlowControl m_flow       {FlowControl::NONE}; //!< flow control type
                cc_t        m_xOnChar    {0x11};      //!< xon charcter default DC1, if use flow control with XON/XOFF
                cc_t        m_xOffChar   {0x13};      //!< xoff charcter default DC3, if use flow control with XON/XOFF
                bool        m_useEOF     {false};     //!< detect EOF?
                cc_t        m_eof        {0xff};      //!< eof charcter defualt EOF(0xff), if use EOF
                int         m_bufferSize {256};       //!< buffer size default 256 byte
                int         m_timeout    {10};        //!< master time out value in mili sec
            }; //<-- struct ConnectionConditions ends here.
            /*! IOSYS (manupilater termios) .
             */
            class iosys
            {
            public:
                using termios_type = struct termios;
                using termios_ptr  = std::unique_ptr<termios_type>;
                iosys()
                    : m_termios {std::make_unique<termios_type>()}
                    , m_save {std::make_unique<termios_type>()}
                {}
                iosys(const iosys&) = delete;
                iosys(iosys&&) noexcept = delete;

                auto setup(const ConnectionConditions* params)
                {
                    // execute cfmakeraw()
                    // termios->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                    // 						| INLCR | IGNCR | ICRNL | IXON);
                    // termios->c_oflag &= ~OPOST;
                    // termios->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
                    // termios->c_cflag &= ~(CSIZE | PARENB);
                    // termios->c_cflag |= CS8;
                    ::cfmakeraw( m_termios.get() ); // initialize termios
                    //m_termios->c_iflag &= ~(ECHO | ECHONL | ECHOK | ECHOE);
                    //m_termios->c_iflag |= IGNBRK;  // ignore break
                    m_termios->c_lflag &= ~(ICANON);              // none line mode
                    m_termios->c_cc[VEOF]  = params->m_eof;
                    //m_termios->c_cc[VMIN]  = params->bufferSize;
                    m_termios->c_cc[VMIN]     = 0;
                    //m_termios->c_cc[VTIME]    = params->timeout * 100; // convert from ms to 1/10 s see man(3) TERMIOS
                    m_termios->c_cc[VTIME]    = 0; // convert from ms to 1/10 s see man(3) TERMIOS
                    m_termios->c_cc[VINTR]    = 0;     /* Ctrl-c */
                    m_termios->c_cc[VQUIT]    = 0;     /* Ctrl-\ */
                    m_termios->c_cc[VERASE]   = 0;     /* del */
                    m_termios->c_cc[VKILL]    = 0;     /* @ */
                    //m_termios->c_cc[VSWTC]    = 0;     /* '\0' */
                    m_termios->c_cc[VSTART]   = 0;     /* Ctrl-q */
                    m_termios->c_cc[VSTOP]    = 0;     /* Ctrl-s */
                    m_termios->c_cc[VSUSP]    = 0;     /* Ctrl-z */
                    m_termios->c_cc[VEOL]     = 0;     /* '\0' */
                    m_termios->c_cc[VREPRINT] = 0;     /* Ctrl-r */
                    m_termios->c_cc[VDISCARD] = 0;     /* Ctrl-u */
                    //m_termios->c_cc[VWERASE]  = 0;     /* Ctrl-w */
                    m_termios->c_cc[VLNEXT]   = 0;     /* Ctrl-v */
                    m_termios->c_cc[VEOL2]    = 0;     /* '\0' */
                    //
                    auto ptr = m_termios.get();
                    cfsetspeed(ptr, baudrate(params->m_baudrate));
                    // CREAD   : receiving characters
                    // CLOCAL  : no modem
                    m_termios->c_cflag |= (CREAD | CLOCAL);
                    // char size
                    m_termios->c_cflag &= ~(CSIZE);
                    m_termios->c_cflag |= char_size(params->m_width);
                    // parity
                    m_termios->c_iflag &= ~(PARMRK | INPCK);
                    m_termios->c_iflag |= IGNPAR;
                    switch (params->m_parity) {
                    case Parity::EVEN:
                        m_termios->c_cflag &= ~PARODD;
                        m_termios->c_cflag |= PARENB;
                        break;
                    case Parity::ODD:
                        m_termios->c_cflag |= PARENB | PARODD;
                        break;
                    case Parity::NONE:
                    default:
                        m_termios->c_cflag &= ~PARENB;
                        break;
                    }
                    // start bits
                    // start bit is FIXed 1bit
                    // stop bits
                    if (params->m_stop == StopBit::ONE) {
                        m_termios->c_cflag &= ~(CSTOPB);
                    } else {
                        m_termios->c_cflag |= CSTOPB;
                    }
                    // flowcontrol
                    switch (params->m_flow) {
                    case FlowControl::RTS_CTS:
                        m_termios->c_cflag |= CRTSCTS;
                        m_termios->c_iflag &= ~(IXON | IXOFF | IXANY);
                        break;
                    case FlowControl::XON_XOFF:
                        m_termios->c_cflag &= ~CRTSCTS;
                        m_termios->c_iflag |= IXON | IXOFF | IXANY;
                        m_termios->c_cc[VSTART] = params->m_xOnChar;
                        m_termios->c_cc[VSTOP]  = params->m_xOffChar;
                        break;
                    case FlowControl::NONE:
                    default:
                        m_termios->c_cflag &= ~CRTSCTS;
                        m_termios->c_iflag &= ~(IXON | IXOFF | IXANY);
                        break;
                    }
                    return Sml::OK;
                }
                /*! Change current termios from new setup .
                 */
                auto changeTermios(fd_type fd) noexcept
                {
                    ::fcntl(fd, F_SETOWN, ::getpid());
                    /// set up to fd non blocking mode.
                    /// Command F_SETFL only mode O_APPEND and O_NONBLOCK
                    ::fcntl(fd , F_SETFL , O_NONBLOCK);

                    ::tcflush(fd, TCIFLUSH);
                    ::tcflush(fd, TCOFLUSH);
                    ::tcgetattr(fd, m_save.get()); // save current termios
                    ::tcsetattr(fd, TCSANOW, m_termios.get()); // change current to new termios
                    return Sml::OK;

                }
                /*! Restore old termios .
                 */
                auto restoreTermios(fd_type fd) noexcept
                {
                    if (fd > 2) {
                        m_save->c_cflag &= ~(HUPCL); // This to release the RTS after close
                        ::tcsetattr(fd, TCSANOW, m_save.get());
                    }
                }
            private:
                termios_ptr m_termios; //!< terminal configuration
                termios_ptr m_save; //!< terminal configuration
            }; //<-- class iosys ends here.
            /*!  Serial port parameters.
             */
            class Parameters : public ConnectionParameterBase
            {
            public:
                using iosys_ptr     = std::unique_ptr<iosys>;
                using property_type = ConnectionConditions;
                using property_ptr  = std::unique_ptr<property_type>;
                /*! Constructor (default).
                 */
                Parameters()
                    : Parameters("/dev/ttyS0"s, Baudrate::BPS115200, Parity::NONE, FlowControl::NONE, StopBit::ONE, CharSize::W8) {}
                /*! Constructor with devicename, ConnectionConditions .
                 */
                Parameters(const std::string& name, property_type c)
                    : ConnectionParameterBase {name}
                    , m_iosys {std::make_unique<iosys>()}
                    , m_property {std::make_unique<property_type>(c)}
                {}
                /*! Constructor with device name, baudrate, parity type, flow type, stopbit spec and char size(width).
                 */
                Parameters(const std::string& name, Baudrate baudrate, Parity parity, FlowControl flow, StopBit stop, CharSize width)
                    : ConnectionParameterBase {name}
                    , m_iosys {std::make_unique<iosys>()}
                    , m_property {std::make_unique<property_type>()}
                {
                    m_property->m_baudrate = baudrate;
                    m_property->m_parity   = parity;
                    m_property->m_flow     = flow;
                    m_property->m_stop     = stop;
                    m_property->m_width    = width;
                }
                /*! Constructor with device name, baudrate, parity type, flow type, stopbit spec and char size(width).
                 */
                Parameters(const std::string& name, Baudrate baudrate, Parity parity, FlowControl flow)
                    : Parameters(name, baudrate, parity, flow, StopBit::ONE, CharSize::W8) {}
                /*! Constructor with device name, baudrate, parity type, flow type, stopbit spec and char size(width).
                 */
                Parameters(const std::string& name, Baudrate baudrate, Parity parity)
                    : Parameters(name, baudrate, parity, FlowControl::NONE, StopBit::ONE, CharSize::W8) {}
                /*! Constructor with device name, baudrate, parity type, flow type, stopbit spec and char size(width).
                 */
                Parameters(const std::string& name, Baudrate baudrate)
                    : Parameters(name, baudrate, Parity::NONE, FlowControl::NONE, StopBit::ONE, CharSize::W8) {}
                /*! Constructor with device name, baudrate, parity type, flow type, stopbit spec and char size(width).
                 */
                Parameters(const std::string& name)
                    : Parameters(name, Baudrate::BPS115200, Parity::NONE, FlowControl::NONE, StopBit::ONE, CharSize::W8) {}
                /*! Copy constructor (delete) .
                 */
                Parameters(const Parameters&) = delete;
                /*! Move constructor .
                 */
                Parameters(Parameters&& rhs) noexcept
                {
                    if (this != &rhs) {
                        m_iosys    = std::move(rhs.m_iosys);
                        m_property = std::move(rhs.m_property);
                    }
                }
                /*! Copy assign (delete) .
                 */
                Parameters& operator=(const Parameters) = delete;
                /*! Move assign .
                 */
                Parameters& operator=(Parameters&& rhs) noexcept
                {
                    if (this != &rhs) {
                        m_iosys    = std::move(rhs.m_iosys);
                        m_property = std::move(rhs.m_property);
                    }
                    return *this;
                }
                ~Parameters() = default;
                auto setup(fd_type fd)
                {
                    m_iosys->setup(m_property.get());
                    m_iosys->changeTermios(fd);
                }
                /*! Change parm .
                 */
                auto change(Baudrate      baudrate = Baudrate::BPS115200
                            , Parity      parity   = Parity::NONE
                            , FlowControl flow     = FlowControl::NONE
                            , StopBit     stop     = StopBit::ONE
                            , CharSize    width    = CharSize::W8
                    ) noexcept -> return_code
                {
                    if (m_property) {
                        m_property->m_baudrate = baudrate;
                        m_property->m_parity   = parity;
                        m_property->m_flow     = flow;
                        m_property->m_stop     = stop;
                        m_property->m_width    = width;
                        return OK;
                    }
                    return FAILURE;
                }
                /*! Device name setter .
                 */
                auto deviceName(const std::string& n) noexcept {name(n);}
                /*! Device name getter .
                 */
                auto deviceName() const noexcept {return name();}
                /*! ConnectionConditions setter (rhs) .
                 */
                auto attachConnectionConditions(property_ptr&& p)
                {
                    m_property = std::move(p);
                }
                /*! ConnectionConditions setter.
                 */
                auto attachConnectionConditions(property_type p)
                {
                    m_property = std::make_unique<property_type>(p);
                }
                auto ioctl() const noexcept -> const iosys* {return m_iosys.get();}
                auto ioctl() noexcept -> iosys* {return m_iosys.get();}
            private:
                iosys_ptr    m_iosys {};
                property_ptr m_property{};
            }; //<-- class Parameters ends here.
        }  // namespace Serial
    }  // namespace IO
}  // namespace Sml
#endif         //<-- macro  PARAMS_Hpp ends here.
/**
 *    @}
 *  @}
 */
