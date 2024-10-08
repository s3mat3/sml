/*!
 * \file base.hpp
 *
 * \copyright © 2024 s3mat3 
 *
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_BASE_Hpp
# define  SML_BASE_Hpp
# include <string>

namespace Sml {

    /*! Sml base class .
     *
     * Abstract Base class for all class
     */
    class Base
    {
    public:
        using id_type = std::size_t;
        Base()
            : Base(0, "no-name") {}
        explicit Base(id_type id)
            : Base(id, "no-name") {}
        explicit Base(const std::string& name)
            : Base(0, name) {}
        explicit Base(id_type id, const std::string& name)
            : m_id(id)
            , m_name(name) {}
        Base(const Base& rhs)
            : Base(rhs.m_id, rhs.m_name) {}
        Base(Base&& rhs) noexcept
            : Base(rhs.m_id) {m_name.assign(std::move(rhs.m_name));}
        Base& operator=(const Base&) = delete;
        virtual ~Base() = default;

        // getter
        id_type id() const noexcept {return m_id;}
        const std::string name() const noexcept {return m_name;}
        // setter
        void id(id_type newID) noexcept {m_id = newID;}
        void name(const std::string& name) noexcept {m_name.assign(name);}
        void name(std::string&& name) noexcept {m_name.assign(std::move(name));}
        // manipulator for data
        void clear() noexcept
        {
            m_id = 0;
            m_name.clear();
        }
    private:
        id_type     m_id;
        std::string m_name;
    };

} //<-- namespace Sml ends here.

#endif //<-- macro  SML_BASE_Hpp ends here.
