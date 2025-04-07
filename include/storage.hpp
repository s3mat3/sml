/*!
 * \file storage.hpp
 *
 * \copyright © 2025 s3mat3
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * \brief storag base class
 *
 * \author s3mat3
 */

#pragma once

#ifndef STORAGE_Hpp
# define  STORAGE_Hpp

# include <cstdlib>
# include <initializer_list>
# include <memory>
# include <type_traits>

# include "sml.hpp"

# include "debug.hpp"

namespace Sml {
    static constexpr size_type ZERO = 0; //!< type hint for size_type
    enum class rooms : size_type {
        V64  =    64,
        V128 =   128,
        V256 =   256,
        V512 =   512,
        V1K  =  1024,
        V2K  =  2048,
        V4K  =  4096,
        V8K  =  8192,
        V16K = 16384,
    };
    static constexpr size_type request_volume(rooms r) {return static_cast<size_type>(r);}
    static constexpr size_type default_volume() {return request_volume(rooms::V64);}

    template <typename T>
    using is_storage_contents_requirement = std::conjunction<
        std::is_default_constructible<T>
        , std::is_copy_constructible<T>
        , std::is_move_constructible<T>
        >;

    /*!  Storage base class.
     *
     * Allocatable storage
     * allocate size of content(s) x rooms volume
     * First allocate the size of content(s) times volume and after construct rooms for content in constructor
     * \note Only std::allocator is considered ...
     * \code
     * storage ###########.........# (after construct)
     *         ^                   ^
     *         |                   |
     * head----+                   +----end
     * tail----+
     *
     * storage xx#########.........# (after append 2 contens)
     *         ^ ^                 ^
     *         | |                 |
     * head----+ |                 +----end
     * tail------+
     * \endcode
     */
    template <typename T, typename A = std::allocator<T>>
    class StorageBase
    {
        static_assert(is_storage_contents_requirement<T>::value, "Necessary the typename T can be copy and move constructible and default constructible");
        using value_type      = T;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;
        using iterator        = pointer;
        using const_iterator  = const_pointer;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using rvalue_type     = value_type&&;
        using allocator_type  = A;
        using init_list_type  = std::initializer_list<value_type>; 
    public:
        //
        // StorageBase() = default;
        /*! Constractor 1 (default only reserve 64rooms).
         */
        constexpr StorageBase() : StorageBase(default_volume())
        {
            // TRACE("ctor 1");
        }
        /*! Constractor 2 Only reserve.
         */
        constexpr StorageBase(size_type s)
        {
            // TRACE("ctor 2");
            reserve(s);
        }
        /*! Constructor 3 fill 1 object (const reference) .
         * \note size() == capacity()
         */
        explicit constexpr StorageBase(size_type s, const_reference v)
        {
            // TRACE("ctor 3");
            reserve(s);
            auto ptr = m_head;
            for (auto i = ZERO; i < s; ++i, ++ptr, ++m_tail) {
                *ptr = v;
            }
        }
        /*! Constructor 4 fill 1 object (rvalue reference) .
         * \note size() == capacity()
         */
        explicit constexpr StorageBase(size_type s, rvalue_type v)
        {
            // TRACE("ctor 4");
            reserve(s);
            auto ptr = m_head;
            for (auto i = ZERO; i < s; ++i, ++ptr, ++m_tail) {
                *ptr = v;
            }
        }
        /*! Constructor initializer list .
         *  \note not explicit StorageBase<T> x = {1,2,3,4,5};
         */
        constexpr StorageBase(init_list_type l)
        {
            // TRACE("ctor 5 initializer list");
            reserve(l.size());
            for (auto x : l) {
                *m_tail++ = x;
            }
        }
        /*! Copy constructor .
         */
        explicit StorageBase(const StorageBase& rhs)
        {
            // TRACE("ctor copy");
            if (this != &rhs) {
                reserve(rhs.capacity());
                copy(rhs);
            }
        }
        /*! Move constructor .
         */
        explicit StorageBase(StorageBase&& rhs) noexcept
            : m_head(rhs.m_head)
            , m_tail(rhs.m_tail)
            , m_end(rhs.m_end)
            , m_capacity(rhs.m_capacity)
            , m_at(rhs.m_at)
            , m_init(rhs.m_init)
        {
            // TRACE("ctor move");
            rhs.m_head = nullptr;
            rhs.m_tail = nullptr;
            rhs.m_end  = nullptr;
            rhs.m_init = false;
        }
        /*! Copy assign operator.
         */
        StorageBase& operator=(const StorageBase& rhs)
        {
            // TRACE("copy assign");
            if (this != &rhs && m_capacity >= rhs.m_capacity) {
                copy(rhs);
            }
            return *this;
        }
        /*! Move assign operator .
         */
        StorageBase& operator=(StorageBase&& rhs) noexcept
        {
            // TRACE("move assign");
            if (this != &rhs && this->m_capacity >= rhs.m_capacity) {
                destroy_all();
                deallocate();
                m_head     = rhs.m_head;
                m_tail     = rhs.m_tail;
                m_end      = rhs.m_end;
                m_at       = rhs.m_at;
                m_init     = true;
                // clear rhs but no call destructor
                rhs.m_init = false;
                rhs.m_head = nullptr;
                rhs.m_tail = nullptr;
                rhs.m_end  = nullptr;
            }
            return *this;
        }
        /*!  destroy all.
         */
        ~StorageBase()
        {
            if (m_init) {
                destroy_all();
                deallocate();
                m_init = false;
            }
        }
        /*! Evalute allocated or not .
         */
        operator bool() const noexcept {return m_init;}
        /*! Evalute allocated or not .
         */
        constexpr auto is_inited() const noexcept {return m_init;}
        /*! Get storage size .
         */
        constexpr auto capacity() const noexcept -> size_type {return m_capacity;}
        /*! Get number of stored conten(s) .
         */
        auto size() const noexcept -> size_type {return m_tail - m_head;}
        /*! Check no content .
         */
        auto empty() const noexcept -> bool {return m_tail == m_head;}
        /*! Check usable rooms .
         */
        auto has_rooms() const noexcept -> bool {return m_tail < m_end;}
        /*! Check contents is full .
         */
        auto full() const noexcept -> bool {return m_tail == m_end;}
        /*! Check storag overflow after one content .
         */
        auto overflow() const noexcept -> bool {return overflow(1);}
        /*! Check storage overflow after l conten(s) .
         */
        auto overflow(size_type l) const noexcept -> bool {return (m_tail + l) > m_end;}
        /*! Get raw pointer of storage head .
         */
        auto ptr() noexcept -> pointer {return m_head;}
        /*! Get raw pointer of storage head (const) .
         */
        auto const_ptr() const noexcept -> const value_type* {return m_head;}
        /*! Get begin iterator .
         */
        auto begin() noexcept -> iterator {return m_head;}
        /*! Get begin iterator (const) .
         */
        auto const_begin() const noexcept -> const value_type* {return m_head;}
        /*! Get end iterator .
         */
        auto end() noexcept -> iterator {return m_tail;}
        /*! Get end iterator (const) .
         */
        auto const_end() const noexcept -> const value_type* {return m_tail;}
        /*!  Copy from value_type array (reference version).
         */
        auto copy_from(const_reference v, size_type l)
        {
            for (size_type i = ZERO; i != l; ++i) {
                m_head[i] = v[i];
            }
            update_tail(l);
        }
        /*! Copy from value_type array (pointer version) .
         */
        auto copy_from(const value_type* p, size_type l)
        {
            auto ptr = p;
            for (size_type i = ZERO; i != l; ++i, ++ptr) {
                m_head[i] = *ptr;
            }
            update_tail(l);
        }
    protected:
        using traits = std::allocator_traits<allocator_type>;
        /*! Tail pointer updater .
         */
        auto update_tail(size_type l) noexcept {m_tail += l;}
        /*!  Copy storage.
         */
        auto copy(const StorageBase& rhs) -> void
        {
            std::copy(rhs.const_begin(), rhs.const_end(), begin());
            update_tail(rhs.size());
        }
        /*! Copy storage .
         */
        auto copy(const_iterator b, const_iterator e) -> void
        {
            std::copy(b, e, begin());
            update_tail(std::distance(b, e));
        }
        /*! Initialize pointers .
         *
         * Create tail and end from head and capacity from given size
         *  \param[in] s Given storage size
         */
        auto initialize(size_type s) noexcept
        {
            m_tail = m_head;
            m_end = m_head + s;
            m_capacity = s;
            m_init = true;
        }
        /*! Reserve memory .
         *
         * allocate and construct(need default constructor)
         * \note Some times throw bad_alloc from allocator ...
         *  \param[in] s is requested volume of rooms for resouce
         *  \retval OK allcated
         *  \retval NO_RESOURCE when catch bad_alloc() from allocate class
         */
        auto reserve(size_type s) noexcept -> return_code
        {
            // TRACE("reserve");
            if (s == ZERO) return OK;
            try {
                m_head = allocate(s);
            } catch (std::bad_alloc& e) {
                SML_FATAL(e.what());
                return NO_RESOURCE;
            }
            initialize(s);
            auto ptr = m_head;
            for (size_type i = 0; i != s; ++i, ++ptr) {
                construct(ptr);
            }
            return OK;
        }
        /*! Reserve memory for initializer list but It's not works :-) .
         *
         * \todo it's works
         *  \tparam ...Args is initializer list data cf {1,2,3,4,5,6} etc
         *  \param[in] s is requested volume of rooms for resouce
         *  \param[in] ...args are contents of initializer
         */
        template <class...Args>
        auto reserve(size_type s, Args&&... args)
        {
            // TRACE("reserve Initializer list");
            if (s == ZERO) return;
            m_head = allocate(s);
            if (m_head) {
                initialize(s);
                auto ptr = m_head;
                for (size_type i = 0; i != s; ++i, ++ptr) {
                    construct(ptr, args...);
                }
            }
            return OK;
        }
        /*! Resizing storage .
         *
         * copy to tempolary self, destroy self, new reserve 
         * and copy to new storage
         * 
         *  \param[in] s is requested volume of rooms for resouce
         */
        auto resize(size_type s) noexcept -> void
        {
            // if (capacity() >= s) {return;}
            if (is_inited()) {
                StorageBase t(*this);
                destroy_all();
                deallocate();
                reserve(m_capacity + s);
                copy(t);
            } else {
                reserve(s);
            }
        }
        /*! Resize storage twice .
         *
         * new capacity = now capacity + now capacity
         */
        auto resize() noexcept -> void
        {
            resize(m_capacity * 2);
        }
        /*! Shrink .
         *
         * capacity shrink to the size (m_head - m_ tail)
         */
        auto shrink_to_fit() noexcept -> void
        {
            if (capacity() <= size() && size() != 0) return;
            if (is_inited()) {
                StorageBase t(*this);
                destroy_all();
                deallocate();
                reserve(size());
                copy(t);
            }
        }
        /*! Allocate storage .
         *
         *  \param[in] req is requested volume of rooms for resouce
         */
        auto allocate(size_type req) -> pointer { return static_cast<pointer>(traits::allocate(m_at, req));}
        /*! Deallocate storage .
         */
        auto deallocate() -> void {traits::deallocate(m_at, m_head, m_capacity);}
        /*! Construct storage room for continar item .
         *
         *  \param[in] ptr is head address of allocated storage.
         */
        auto construct(pointer ptr) ->void {traits::construct(m_at, ptr);}
        /*! Construct storage room for continar item .
         *
         *
         */
        auto construct(pointer ptr, const_reference value) ->void {traits::construct(m_at, ptr, value);}
        /*! Construct storage room for continar item .
         *
         *
         */
        auto construct(pointer ptr, rvalue_type value) -> void {traits::construct(m_at, ptr, std::move(value));}

        template <class... Args> auto construct(pointer ptr, Args&&... args) -> void {traits::construct(m_at, ptr, std::forward<Args>(args)...);}
        /*! Destruct storage room for continar item .
         *
         *
         */
        auto destroy(pointer ptr) -> void {traits::destroy(m_at, ptr);}
        /*! Destruct storage rooms .
         *
         *
         */
        auto destroy_all() -> void
        {
            //if (! size()) return;// if size = 0 not constructed conteinar's item
            auto ptr = m_end;
            do {
                if (--ptr < m_head) break;
                destroy(ptr);
            } while (1);
            m_tail = m_head; // cause m_tail < m_head
        }
        // members
        pointer        m_head     {nullptr};          //!< storage pointer head
        pointer        m_tail     {nullptr};          //!< storage pointer tail
        pointer        m_end      {nullptr};          //!< storage pointer for point to capacity
        size_type      m_capacity {0};                //!< capacity index
        allocator_type m_at       {allocator_type()}; //!< allocaotr default std::allocator<T>
        bool           m_init     {false};            //!< flag of allocated
    }; //<-- class StorageBase ends here.
} //<-- namespace Sml ends here.

#endif //<-- macro  STORAGE_Hpp ends here.
