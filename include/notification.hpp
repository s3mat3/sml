/*!
 * \defgroup ds delivery-system 
 * @{
 * \file notification.hpp
 *
 * \copyright © 2024 s3mat3
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief notification class for callback
 *
 * \warning You need a c++11 or higher compiler. For gcc, if it is not g++4.9 or higher, there is a mistake in the functional implementation.
 * \warning Be careful when crossing threads because resource protection is not performed!
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_NOTIFICATION_Hpp
# define  SML_NOTIFICATION_Hpp

# include <functional>
# include <memory>

# include "debug.hpp"


namespace Sml {
    /*!Notification class
     *
     * This class can notify even if the delivery destination (callback) is unknown.
     * Even in this state, without crashing, display an error message and continue the program.
     * A little connect can be used as a complicated and difficult but flexible event delivery system.
     * \tparam Args Argument type of a member function (callback) that receives an event (do not write it if variable length is void)
     */
    template <typename... Args>
    class Notification
    {
    public:
        using reciver_type = std::function<return_code(Args...)>; //!< callback
    public:
        /*! Default constructor .
         */
        Notification() = default;
        /*! Constructor 2.
         *  \tparam T target function (callback) that receives an Notification<>::notify
         *  \tparam ...A Argument(s) type of a member function (callback) that receives an event (do not write it if variable length is void)
         */
        // template <typename T, typename... A>
        // constexpr Notification(T func, A... a) : m_reciver {std::bind(func, a...)} {}
        template <typename T, typename... A>
        constexpr Notification(T& func, A... a) : m_reciver {std::bind(func, a...)} {}
        /*! Constructor 3.
         *  \tparam T target function (callback) that receives an Notification<>::notify
         *  \tparam ...A Argument(s) type of a member function (callback) that receives an event (do not write it if variable length is void)
         */
        template <typename T, typename... A>
        constexpr Notification(T&& func, A... a) : m_reciver {std::bind(func, a...)} {}
        /*! Constructor 4.
         * argument void case
         *  \tparam T target function (callback) that receives an Notification<>::notify
         */
        template <typename T>
        constexpr Notification(T& func) : m_reciver {func} {}
        /*! Constructor 5.
         * argument void case
         *  \tparam T target function (callback) that receives an Notification<>::notify
         */
        template <typename T>
        constexpr Notification(T&& func) : m_reciver {func} {}
        /*! Constructor 6.
         *  \tparam C Class to receive event (class which defined callback)
         *  \tparam T target member function (callback) that receives an Notification<>::notify
         *  \tparam ...A Argument(s) type of a member function (callback) that receives an event (do not write it if variable length is void)
         */
        template <class C, typename T, typename... A>
        constexpr Notification(T&& mf, C* instance, A... a) : m_reciver {std::bind(std::forward<T>(mf), instance, a...)} {}
        /*! Constructor 7.
         *  \tparam C Class to receive event (class which defined callback)
         *  \tparam T target member function (callback) that receives an Notification<>::notify
         *  \tparam ...A Argument(s) type of a member function (callback) that receives an event (do not write it if variable length is void)
         */
        template <class C, typename T, typename... A>
        constexpr Notification(T&& mf, std::shared_ptr<C> instance, A... a) : Notification(mf, instance.get(), a...) {}
        /*! Move constructor .
         */
        Notification(Notification&& rhs) noexcept
        {
            if (this != &rhs) {std::swap(m_reciver, rhs.m_reciver);}
        }
        /*! Move assigen operator.
         */
        Notification& operator=(Notification&& rhs) noexcept
        {
            if (this != &rhs) {m_reciver = std::move(rhs.m_reciver); rhs.m_reciver = nullptr;}
        }
        /// destructor is defalut
        ~Notification() = default;
        /// copy constructor deleted
        Notification(const Notification&) = delete;
        /// copy assign deleted
        Notification& operator=(const Notification&) = delete;
        /*! Connect notify destination (callback) (raw pointer version)
         *
         *  \tparam C Class to receive event (class which defined callback)
         *  \tparam T Member function (callback) that receives an event of class C
         *  \tparam ...A Argument(s) type of a member function (callback) that receives an event (do not write it if variable length is void)
         *
         *  \param[in] mf Connection destination (callback) by member function of obj
         *  \param[in] instance of class pointer in which connection destination is defined
         *  \param[in] a Arguments of member functions to be called back (variable length) Perhaps you can pass arguments at runtime by using the SML_ARG (x) macro
         */
        template <class C, typename T, typename... A>
        constexpr auto connect(T&& mf, C* instance, A... a) noexcept {m_reciver = std::bind(std::forward<T>(mf), instance,  a...);}
        /*! Connect notify destination (callback) shared pointer version
         *
         *  \tparam C Class to receive event (class which defined callback)
         *  \tparam T Member function (callback) that receives an event of class C
         *  \tparam ...A Argument(s) type of a member function (callback) that receives an event (do not write it if variable length is void)
         *
         *  \param[in] mf Connection destination (callback) by member function of obj
         *  \param[in] instance  of class pointer in which connection destination is defined
         *  \param[in] a Arguments of member functions to be called back (variable length) Perhaps you can pass arguments at runtime by using the SML_ARG (x) macro
         */
        template <class C, typename T, typename... A>
        constexpr auto connect(T&& mf, std::shared_ptr<C> instance, A... a) noexcept {m_reciver = std::bind(mf, instance.get(),  a...);}//{connect(mf, instance.get(), a...);}
        //
        template <typename T, typename... A>
        constexpr auto connect(T func, A... a) noexcept {m_reciver = std::bind(func, a...);}

        template <typename T>
        constexpr auto connect(T func) noexcept {m_reciver =func;}

        /*! Execute callback(reciver function)
         *
         *  Call the connected callback
         *
         *  \param[inout] args is argument(s) for callback
         *  \retval OK success callback function
         */
        auto notify(Args... args) noexcept
        {
            int ret = FAILURE;
            if (m_reciver) { // is valid reciver
                try {
                    ret = m_reciver(args...);
                } catch (std::bad_function_call& e) { //
                    SML_FATAL("=====> Occure Exception std::bad_function_call");
                } catch (...) {
                    SML_FATAL("=====> Occure Exception!! What huppen???");
                }
            } else {
                SML_ERROR("====> Notification :: No assignment receiver function(no callback)!! please setup me!!");
            }
            return ret;
        }
        /*! Functor for notify .
         */
        auto operator()(Args... args) noexcept {return notify(args...);}
        /*!  Valid reciver check.
         */
        operator bool() const noexcept {return m_reciver != nullptr;}
    private:
        reciver_type m_reciver {nullptr}; //!< notify reciver
    }; //<-- class Notification ends here.
    /*! \class Notification
     *
     * \example notification/example.cpp
     */
    //
    /// none member functions
    //

    /*! Notification connect helper function
     *
     *  \tparam C Class to receive event (class which defined callback)
     *  \tparam T Member function (callback) that receives an event of class C
     *  \tparam ...A Argument(s) type of notify function of Notification class of connection source
     *  \tparam ...AA Argument(s) type of notify function of Notification class of connection source
     *
     *  \param[in] n Notification class references of connection source (not const because cheange to source of Notification member)
     *  \param[in] instance of class pointer in which connection destination is defined
     *  \param[in] mf Connection destination (callback) by member function of instance
     *  \param[in] a Arguments of member functions to be called back (variable length) Perhaps you can pass arguments at runtime by using the SML_ARG (x) macro
     */
    template <class C, typename T, typename...A, typename... AA>
    inline auto connect(Notification<A...>& n, T&& mf, C* instance, AA... a) {n.connect(mf, instance, a...);}
    template <class C, typename T, typename...A, typename... AA>
    inline auto connect(Notification<A...>& n, T&& mf, std::shared_ptr<C> instance, AA... a) {n.connect(mf, instance, a...);}
    /*! Notification connect helper function
     *
     *  \tparam C Class to receive event (class which defined callback)
     *  \tparam T Member function (callback) that receives an event of class C
     *  \tparam ...A Argument(s) type of notify function of Notification class of connection source
     *  \tparam ...AA Argument(s) type of notify function of Notification class of connection source
     *
     *  \param[in] n Notification class pointer of connection source
     *  \param[in] instance of class pointer in which connection destination is defined
     *  \param[in] mf Connection destination (callback) by member function of instance
     *  \param[in] a Arguments of member functions to be called back (variable length) Perhaps you can pass arguments at runtime by using the SML_ARG (x) macro
     */
    template <class C, typename T, typename...A, typename... AA>
    inline auto connect( Notification<A...>* n, T&& mf, C* instance, AA... a) {n->connect(mf, instance, a...);}
    template <class C, typename T, typename...A, typename... AA>
    inline auto connect( Notification<A...>* n, T&& mf, std::shared_ptr<C> instance, AA... a) {n->connect(mf, instance, a...);}

    /*! Notification connect helper function
     *
     *  \tparam C Class to receive event (class which defined callback)
     *  \tparam T Member function (callback) that receives an event of class C
     *  \tparam ...A Argument(s) type of notify function of Notification class of connection source
     *  \tparam ...AA Argument(s) type of notify function of Notification class of connection source
     *
     *  \param[in] n Notification class pointer of connection source
     *  \param[in] instance of class pointer in which connection destination is defined
     *  \param[in] mf Connection destination (callback) by member function of instance
     *  \param[in] a Arguments of member functions to be called back (variable length) Perhaps you can pass arguments at runtime by using the SML_ARG (x) macro
     */
    template <class C, typename T, typename... A, typename... AA>
    inline void connect( Notification<A...>* n, T&& mf, C* instance, AA... a) {n->connect(mf, instance, a...);}
    template <class C, typename T, typename... A, typename... AA>
    inline void connect( Notification<A...>* n, T&& mf, std::shared_ptr<C> instance, AA... a) {n->connect(mf, instance, a...);}

    /*! Notification connect helper function
     *
     *  \tparam C Class to receive event (class which defined callback)
     *  \tparam T Member function (callback) that receives an event of class C
     *  \tparam ...A Argument(s) type of notify function of Notification class of connection source
     *  \tparam ...AA Argument(s) type of notify function of Notification class of connection source
     *
     *  \param[in] n Notification class pointer of connection source
     *  \param[in] instance of class pointer in which connection destination is defined
     *  \param[in] mf Connection destination (callback) by member function of instance
     *  \param[in] a Arguments of member functions to be called back (variable length) Perhaps you can pass arguments at runtime by using the SML_ARG (x) macro
     */
    template <class C, typename T, typename... A, typename... AA>
    inline void connect(std::shared_ptr<Notification<A...>> n, T&& mf, C* instance, AA... a) {n->connect(mf, instance, a...);}
    template <class C, typename T, typename... A, typename... AA>
    inline void connect(std::shared_ptr<Notification<A...>> n, T&& mf, std::shared_ptr<C> instance, AA... a) {n->connect(mf, instance, a...);}


    template <typename T, typename... A, typename... AA>
    inline void connect(Notification<A...>* n, T mf, AA... a) {n->connect(mf, a...);}
    template <typename T, typename... A, typename... AA>
    inline void connect(Notification<A...> *n, T &&mf, AA... a) {
      n->connect(mf, a...);
    }
    } // namespace Sml

#endif //<-- macro  SML_NOTIFICATION_Hpp ends here.
/** @} */
