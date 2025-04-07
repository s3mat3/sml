/*!
 * \file byte_buffer.hpp
 *
 * \copyright © 2025 s3mat3
 *
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * \brief Buffer for byte(char type)
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_BUFFER_Hpp
# define  SML_BUFFER_Hpp

# include "buffer.hpp"

namespace Sml {

    using ByteBuffer = BufferBase<char>;
    /*!  Alias for ByteBuffer .
     */
    //using ByteBuffer = BufferBase<char>;
    /*! ByteBuffer to std::string .
     */
    std::string to_string(const ByteBuffer& b)
    {
        return std::string(b.const_ptr(), b.size());
    }
    /*! std::string to ByteBuffer .
     */
    ByteBuffer from_string(const std::string& s)
    {
        auto b = ByteBuffer(s.size());
        b.copy_from(s.data(), s.size());
        return b;
    }

    inline auto operator==(const ByteBuffer& lhs, const std::string rhs) -> bool
    {
        return (to_string(lhs) == rhs);
    }
    inline auto operator==(const std::string& lhs, const ByteBuffer& rhs) -> bool
    {
        return (to_string(rhs) == lhs);
    }
    /*! ByteBuffer to string in hex dump  .
     */
    inline std::string hexDump(ByteBuffer& t)
    {
        std::string d {};
        for (const auto& x : t) {
            d.append(Debug::hexChar256Tbl[(static_cast<std::uint8_t>(x & 0xff))]);
        }
        return d;
    }
    /*! ByteBuffer to string in human readable .
     */
    inline std::string toReadableCtrlCode(ByteBuffer& s)
    {
        std::string d{};
        std::string t;
        for (auto c : s) {
            auto x = (static_cast<size_type>(c)) & 0xff;
            if (x < 0x20) {
                d += Debug::ctrlCharTbl[x];
            } else if (x == 0x20) {
                d.append("[SPC]");
            } else if (x == 0x7f) {
                d.append("[DEL]");
            } else if (x > 0x7f) {
                if (x == 0xff) {
                    d.append("[EOF]");
                } else {
                    d.append(Debug::ctrlCharTbl[x - 0x60]);
                }
            } else {
                d.push_back(x);
            }
        }
        return d;
    } //<-- function toReadableCtrlCode ends here.
} //<-- namespace Sml ends here.


#endif //<-- macro  SML_BUFFER_Hpp ends here.
