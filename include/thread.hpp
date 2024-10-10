/*!
 * \file thread.hpp
 *
 * \copyright © 2024 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Thread helpers
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_THREAD_Hpp
# define  SML_THREAD_Hpp

# include <memory>
# include <thread>

# include "base.hpp"
# include "debug.hpp"

namespace Sml{
    /*!  Runnable Interface class.
     */
    class Runnable
    {
    public:
        virtual ~Runnable() {}
        virtual void run(void*) noexcept = 0;
        virtual return_code stop(void) noexcept = 0;
    }; //<-- class Runnable ends here.

    /*!  Runnableadapter
     *
     * The condition for attaching is that return_code stop() must be implemented in the class member function.
     * Also, a member function corresponding to entrypoint_t executed in run(VP) must be implemented.
     * The Thread class launches the function of any class attached to this class as a separate thread space.
     */
    template <class C>
    class RunnableAdapter : public Runnable
    {
    public:
        using entrypoint_t = void (C::*)(void_ptr);
        using instance_p = std::shared_ptr<C>;

        explicit RunnableAdapter(instance_p target, entrypoint_t func) noexcept
            : m_instance(std::move(target))
            , m_entrypoint(func)
        {}
        RunnableAdapter() : RunnableAdapter(nullptr, nullptr) {}

        RunnableAdapter(const RunnableAdapter&) = delete;

        RunnableAdapter(RunnableAdapter&& rhs) noexcept
            : RunnableAdapter(std::move(rhs.m_instance), rhs.m_entrypoint) {}

        virtual ~RunnableAdapter() = default;

        virtual void run(void_ptr vp) noexcept override
        {
            if (m_instance && m_entrypoint) {
                (*m_instance.*m_entrypoint)(vp);
            } else {
                SML_FATAL("=====> Hasn't runnable entrypoint object");
            }
        }

        virtual return_code stop() noexcept override
        {
            m_instance->stop();
            SML_LOG("stoped");
            return OK;
        }

        void attach(instance_p obj, entrypoint_t entrypoint)
        {
            m_instance   = obj;
            m_entrypoint = entrypoint;
        }

        void operator ()(instance_p obj, entrypoint_t function)
        {
            attach(obj, function);
        }
        Runnable& operator =(RunnableAdapter&& rhs)
        {
            if (this != rhs) {
                m_entrypoint = rhs.m_entrypoint;
                m_instance   = std::move(rhs.m_instance);
            }
            return *this;
        }
        Runnable& operator =(const RunnableAdapter& o)
        {
            if (this != &o) {
                attach(o.m_instance, o.m_function);
            }
            return *this;
        }
    private:
        instance_p   m_instance;    //!< target class instance
        entrypoint_t m_entrypoint;  //!< adapted runner
    }; //<-- class RunnableAdapter ends here.

    /*!  \class Thread
     * \brief Thread class for evry class
     *
     * This class is final. (can not derived)
     *
     * It is simply a helper class to easily migrate a void(*)(void*) member function of a certain class to the thread space.
     */
    class Thread final : public Base
    {
    public:
        using runnable_p = std::shared_ptr<Runnable>;
        using runnable_w = std::weak_ptr<Runnable>;
        using thread_u   = std::unique_ptr<std::thread>;

        Thread(runnable_p runnable, ID_t id, const std::string& name) noexcept
            : Base{id, name}
            , m_runnable{runnable}
            , m_thread{nullptr}
            , m_started{false} {}
        explicit Thread(runnable_p runnable, const std::string& name) noexcept : Thread(runnable, 0, name) {}
        explicit Thread(runnable_p runnable) noexcept : Thread(runnable, "some thread") {}
        Thread() noexcept : Thread(nullptr) {}

        Thread(const Thread&) = delete;

        Thread& operator =(const Thread&) = delete;

        ~Thread() noexcept
        {
            auto c = this->join();
            SML_LOG(name() + " Thread object deleting : " + std::to_string(c));
            m_thread.reset();
            SML_LOG(name() + "::~Thread");
        }
        /*! start threading
         *
         *  runnnable runner go to other thread space
         *  to move another memory context
         *
         *  \param[inout] vp thread argument(s)
         *  \retval OK thread lunched
         *  \retval NO_RESOURCE We have not the RUNNABLE
         */
        auto start(void* vp) noexcept
        {
            return_code ret = NO_RESOURCE;
            if (m_runnable) {
                m_started = true;
                SML_LOG(name() + " start thread");
                m_thread.reset(new std::thread([this,vp]{m_runnable->run(vp);}));
                return OK;
            } else {
                SML_LOG("=====> No setup Runnable object < " + name());
                return ret; // NO_RESOURCE
            }
            return ret; // but naver here.
        }
        /*! set runnable for new thread
         *
         *  \param[in] r for threading target
         */
        void runnable(runnable_p r) noexcept {m_runnable = r;}
        /*! set runnable for new thread
         *
         *  \param[in] r for threading target
         */
        void operator ()(runnable_p r) noexcept {m_runnable = r;}
        /*! Has runnable .
         *
         *  \retval true leagal runnable
         *  \retval false hasn't runnable
         */
        operator bool() const noexcept {return (m_runnable) ? true : false;}
        /*! wait for join terminate thread
         *
         *  \retval OK joined
         *  \retval FAILURE maybe already joined
         *  \retval FAIL_JOIN when catch std::system_error or other
         */
        auto join() noexcept -> return_code
        {
            if (m_thread->joinable() && m_started) {
                try {
                    m_started = false;
                    m_thread->join();
                } catch (std::system_error& e) {
                    SML_FATAL(name() + " =====> fail join : " + e.what());
                    return FAIL_JOIN;
                } catch (...) {
                    SML_FATAL(name() + " =====> fail join : Catch unknown EXCEPTION");
                    return FAIL_JOIN;
                }
                SML_INFO(name() + " => Joined thread");
                return OK;
            } else {
                SML_LOG(name() + " is No joinable maybe alrady joined OR **Not RUN**" );
                return FAILURE;
            }
            return OK; // maybe never here.
        }
        /*! started thread checker
         *
         *  \retval true The thead started
         *  \retval false The thread was joined or Not start
         */
        auto started() const noexcept -> bool {return m_started;}
        /*! sleep current thread
         *
         *  not thread class started thread
         *  only just call current thread
         *  Max limit 2^31[mS]
         *  \param[in] milliseconds in milliseconds unit ([ms])
         */
        static inline void sleep(millisec_interval milliseconds)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        }

        /*! sleep current thread
         *
         *  not thread class started thread
         *  only just call current thread
         *  Max limit 2^31[uS]
         *  \param[in] microseconds in microseconds unit ([us])
         */
        static inline void usleep(time_interval microseconds)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
        }
        /*! yield current thread
         *
         */
        static inline void yield()
        {
            std::this_thread::yield();
        }
    protected:
        runnable_p  m_runnable; //!< real thread runner
        thread_u    m_thread;   //!< thread holder
        bool        m_started;  //!< running flag
    }; // class Thread

    /*! \class Thread
     *
     * \example thread/example.cpp
     */

} //<-- namespace Sml ends here.

#endif //<-- macro  SML_THREAD_Hpp ends here.
