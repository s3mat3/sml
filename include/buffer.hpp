/*!
 * \file buffer.hpp
 *
 * \copyright © 2025 s3mat3
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * \brief buffer base class
 *
 * \author s3mat3
 */

#pragma once

#ifndef BUFFER_Hpp
# define  BUFFER_Hpp
// # define SML_TRACE 1
# include "result.hpp"
# include "storage.hpp"

namespace Sml {
    /*! Buffer base class .
     *
     * Buffer for value_type
     *  \tparam T value_type
     */
    template <typename T>//, size_type N>
    class BufferBase : public StorageBase<T, std::allocator<T>>
    {
    public:
        using value_type             = T;
        using pointer                = value_type*;
        using const_pointer          = const value_type*;
        using reference              = T&;
        using const_reference        = const T&;
        using rvalue_reference       = T&&;
        using const_buffer_reference = const BufferBase<value_type>&;
        using result                 = Result<T>;
        using StorageBase<value_type>::StorageBase;
        using super = StorageBase<value_type>;

        // BufferBase() : StorageBase<value_type>(N) {}
        // auto begin() {return this->m_head;}
        // auto end() {return this->m_tail;}
        /*! Clear buffer .
         *
         * Only pointer operate
         * \note Never delete contents.
         */
        auto clear()
        {
            this->m_tail = this->m_head;
            m_read = ZERO;
        }
        /*! Clear buffer with destruct .
         *
         *
         */
        auto clear_all()
        {
            this->destoroy_all();
        }
        /*!  Content(s) append to tail .
         *
         *
         */
        auto append(const_pointer pv, size_type n) noexcept
        {
            if (this->overflow(n)) this->resize(n);
            std::copy(pv, pv + n, this->m_tail);
            update_tail(n);
            return OK;
        }
        /*!  Content(s) append to tail .
         *
         *
         */
        auto append(const BufferBase& cr) noexcept
        {
            auto n = cr.size();
            if (this->overflow(n)) this->resize(n);
            std::copy(cr.const_begin(), cr.const_end(), this->m_tail);
            update_tail(n);
            return OK;
        }
        /*!  Assign new buffer content(s) from const_pointer.
         *
         *
         */
        auto assign(const_pointer pv, size_type n)
        {
            if (this->capacity() < n) this->resize(n);
            clear();
            this->copy(pv, pv + n);
            return OK;
        }
        /*!  Assing new buffer conten(s) from another buffer reference.
         *
         *
         */
        auto assign(const BufferBase& cr)
        {
            auto n = cr.size();
            if (this->capacity() < n) this->resize(n);
            clear();
            this->copy(cr.const_begin(), cr.const_end());
            return OK;
        }
        /*! Push back 1 object (const value_type&).
         *
         * Append 1 object at storage tail
         *
         * \note When buffer is full, then resizing occur
         *  \param[in] v Target of append
         *  \retval OUT_OF_RANGE index was arlady full
         *  \retval OK appended
         */
        auto push_back(const_reference v) noexcept
        {
            if (this->full()) this->resize();
            *this->m_tail = v;
            ++this->m_tail;
            return Sml::OK;
        }
        /*! Push back 1 object (value_type&&).
         *
         * \note When buffer is full, then resizing occur
         *
         */
        auto push_back(rvalue_reference v) noexcept
        {
            if (this->full()) this->resize();
            *this->m_tail = std::move(v);
            ++this->m_tail;
            return Sml::OK;
        }
        /*! Push back 1 object (const value_type*).
         *
         * \note When buffer is full, then resizing occur
         *
         */
        auto push_back(const_pointer v) noexcept
        {
            if (this->full()) this->resize();
            *this->m_tail = *v;
            ++this->m_tail;
            return Sml::OK;
        }
        /*! Add one object at tail .
         *
         * \note When buffer is full, then resizing occur
         */
        BufferBase& operator+(const_reference v) noexcept
        {
            // BUFFER_CHECK_INDEX_WITH_THROW(this->size());
            if (this->full()) this->resize();
            *this->m_tail = v;
            ++this->m_tail;
            return *this;
        }

        /*! []access .
         *
         * auto y = x[i]
         */
        reference operator[](size_type i) // noexcept
        {
            // if (this->overflow(i)) this->resize();
            if (this->capacity() < i) throw std::out_of_range("Index position is over capacity");
            return this->m_head[i];
        }
        /*! access[] .
         *
         */
        const_reference operator[](size_type i) const // noexcept
        {
            // if (this->overflow(i)) super::resize();
            if (this->capacity() < i) throw std::out_of_range("Index position is over capacity");
            return this->m_head[i];
        }

        auto at(size_type pos) const -> const_reference
        {
            if (this->capacity() < pos) throw std::out_of_range("Index position is over capacity");
            return this->m_head[pos];
        }
        /*! Read buffer data from storage .
         *
         *  \retval value_type
         */
        auto read() -> value_type
        {
            auto pos = m_read;
            if (++m_read > this->size()) throw std::out_of_range("point to no valid data");
            return this->m_head[pos];
        }
        /*! Put back readed object .
         *
         * Only put back position for read data
         *  \retval OK one back
         *  \retval UNDER_FLOW cause read index is alrady first
         */
        auto put_back() noexcept -> return_code
        {
            if (m_read > 0) {
                --m_read;
                return OK;
            } else {
                m_read = 0;
                return UNDER_FLOW;
            }
        }
        /*! Get read starting position .
         *
         *  \retval read position
         */
        auto position() const noexcept
        {
            return m_read;
        }
        /*! Set read starting position .
         *
         *  \param[in] newPos new read position
         *  \retval OK moving
         *  \retval OUT_OF_RANGE newPos is over range
         */
        auto position(size_type newPos) noexcept ->return_code
        {
            if (newPos > this->size()) return OUT_OF_RANGE;
            m_read = newPos;
            return OK;
        }
        /*! Update tail.
         */
        auto update_tail(size_type newPos)
        {
            super::update_tail(newPos);
        }
        /*! Extract buffer data from storage .
         *
         *  \param[in] first extract first position
         *  \param[in] length extract length
         *  \retval error_type out of range (length is over)
         *  \retval extracted buffer
         */
        auto extract(size_type first, size_type length) const noexcept -> Result<BufferBase>
        {
            if (this->size() < (first + length)) return Result<BufferBase>(error_type(OUT_OF_RANGE));
            BufferBase dest(this->m_capacity);
            dest.assign(this->m_head + first, length);
            return dest;
        }
        /*! Extract buffer data from storage .
         *
         *  \param[in] first extract first position
         *  \param[in] length extract length
         *  \retval error_type out of range (length is over)
         *  \retval extracted buffer
         */
        auto substr(size_type first, size_type length) const noexcept -> BufferBase
        {
            if (this->size() < (first + length)) length = this->size() - first;
            BufferBase dest(this->m_capacity);
            dest.assign(this->m_head + first, length);
            return dest;
        }
    private:
        size_type m_read = ZERO; //!< index for read storage
    }; //<-- class BufferBase ends here.
    // template<>
    // class BufferBase<char>
    // {
    // public:
    //     using value_type             = char;
    //     using pointer                = value_type*;
    //     using const_pointer          = const value_type*;
    //     using reference              = char&;
    //     using const_reference        = const char&;
    //     using rvalue_reference       = char&&;
    //     using const_buffer_reference = const BufferBase<value_type>&;
    //     // using result                 = Result<char>;
    //     // using StorageBase<value_type>::StorageBase;
    //     using super = StorageBase<value_type>;
    //     /*! Extract buffer data from storage .
    //      *
    //      *  \param[in] first extract first position
    //      *  \param[in] length extract length
    //      *  \retval error_type out of range (length is over)
    //      *  \retval extracted buffer
    //      */
    //     auto extract(size_type first, size_type length) const noexcept -> Result<BufferBase>
    //     {
    //         if (this->size() < (first + length)) return Result<BufferBase>(error_type(OUT_OF_RANGE));
    //         BufferBase dest(this->m_capacity);
    //         dest.assign(this->m_head + first, length);
    //         return dest;
    //     }
    // };
//    using ByteBuffer = BufferBase<size_type N, char>;
} //<-- namespace Sml ends here.

/*
# define BUFFER_TAIL_CHECK() \
    do{ \
        if ((this->m_end - this->m_tail) <= 0) \
            return Sml::OUT_OF_RANGE; \
    }while(0)
# define BUFFER_TAIL_CHAECK_RESULT()          \
     do{ \
         if ((this->m_end - this->m_tail) <= 0)               \
             return result<T>(error_type(Sml::OUT_OF_RANGE));  \
     }while(0)

# define BUFFER_TAIL_CHECK_INDEX(ind) \
    do{ \
    if (this->m_end - (this->m_tail + ind) <= 0)               \
            return Sml::OUT_OF_RANGE; \
    }while(0)
# define BUFFER_CHECK_INDEX_WITH_THROW(index) \
    do{ \
        if (this->m_capacity <= index) throw std::out_of_range("index is out of range"); \
    } while(0)
*/

#endif //<-- macro  BUFFER_Hpp ends here.
