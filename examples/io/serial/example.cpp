/*!
 * \file example.cpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#include "thread.hpp"
#include "io/serial/port.hpp"


 //  for test on linux by socat command
 //  > socat -d -d -d -d pty,raw,echo=0,link=/tmp/vtty0 pty,raw,echo=0,link=/tmp/vtty1
 //  -d option is debug out many -d very verbose
 //  /tmp/vtty0 for writer
 //  /tmp/vtty1 for reader
 // 

namespace io = Sml::IO;
namespace serial = io::Serial;

static const Sml::count_type LOOP_MAX = 1000;
static Sml::flag_t done(false);

using sio_ptr = std::shared_ptr<serial::Port>;

class Writer{
 public:
    void doWrite(sio_ptr sio)
    {
        std::string data({0x02,'D','E','A','D',0x00,'B','E','E','F',0x00,-1,-127,0x00,0x03});
        std::string msg = "Writer WRITED!!";
        Sml::count_type lc = 0;
        while ((lc++ < LOOP_MAX)) {
            while (done) { Sml::Thread::sleep(1); }
            if (! done) {
                sio->write(data);
                DUMP(Sml::Debug::toReadableCtrlCode(data));
                done = true;
            }
        }
    }
};

class Reader
{
public:
    void doRead(sio_ptr sio)
    {
        Sml::count_type lc = 0;
        while (++lc != LOOP_MAX) {
            while (! done) {Sml::Thread::sleep(1);}
            if (done) {
                bool s = true;
                std::string buf;
                std::string ddd;
                while (s) {
                    buf.clear();
                    auto res = sio->read(buf);
                    if (res > 0) {
                        for (ssize_t i = 0 ;i < res; ++i ) {
                            if (buf[i] == 0x03) {
                                s = false;
                                MARK();
                                break;
                            }
                            ddd.push_back(buf[i]);
                        }
                    }
                }
                DUMP(Sml::Debug::toReadableCtrlCode(ddd));
                ddd.clear();
                done = false;
            }
        }
    }
};


int main()
{
    Writer writer;
    Reader reader;

    auto port1 = std::make_shared<serial::Port>("/tmp/vtty0");
    auto port2 = std::make_shared<serial::Port>("/tmp/vtty1");
    auto p1 = port1->connect();
    auto p2 = port2->connect();
    if (p1 == Sml::OK && p2 == Sml::OK) {
        std::thread wr([&](){ writer.doWrite(port1); });
        std::thread rd([&]() { reader.doRead(port2); });
        if (wr.joinable()) wr.join();
        if (rd.joinable()) rd.join();
    } else {
        SML_FATAL("port not open");
        std::string s = R"(
Let's type
$ socat -d -d -d -d pty,raw,echo=0,link=/tmp/vtty0 pty,raw,echo=0,link=/tmp/vtty1
in console
)";
        MSG(s.data());
    }
    port1->disconnect();
    port2->disconnect();
    return io::IO_OK;
}

