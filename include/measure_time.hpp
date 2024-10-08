/*!
 * \file measure_time.hpp
 *
 * \copyright © 2024 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_MEASURE_TIME_Hpp
# define  SML_MEASURE_TIME_Hpp

# include <chrono>
# include "sml.hpp"

namespace Sml {
    class MeasureTime final
    {
    public:
        using system_clock = std::chrono::steady_clock; //!< steady_clockを選択している!!稼働中に時計を変更されても時間は逆向しない事が保証されているので
        using time_point = std::chrono::time_point<system_clock>;

        explicit MeasureTime(bool m)
                : m_time(system_clock::now())
                , m_flag(m) {}

        MeasureTime() : MeasureTime(true) {}
        MeasureTime(const MeasureTime&) = delete;
        MeasureTime& operator =(const MeasureTime&) = delete;

        ~MeasureTime() = default;
        /*! Start timer.
         *
         *
         */
        void start() noexcept
        {
            m_time = system_clock::now();
            m_flag = true;
        }

        /*! check expire in seconds
         *
         * Check whether the argument is sec or more from start
         *
         *  \param[in] sec in seconds
         *  \param[in] repeate true: ret is true, keep flag start state false: ret is true reset flag 
         *  \retval true equal to or more than, mean "Expired"
         *  \retval false less than
         */
        bool isExpireSec(seconds_interval sec, bool repeate = true) noexcept
        {
            if (sec >= (std::numeric_limits<std::int32_t>::max() / 1000)) {
//                SMLFatal("=====> Targe Seconds very big OVER 24days");
                sec = std::numeric_limits<std::int32_t>::max() / 1000;
                //              SMLFatal("=====> We adjust reduce 24days(2147483.648 = 2147483[s])");
            }
            return isExpire((sec * 1000), repeate);
        }

        /*! check expire in milliseconds
         *
         * Check whether the argument is msec or more from start
         * chrono::millisecondsの実装上の下限値の最大は(45bitの符号付整数なので)2^44が限界値になるがここは
         * 2^31=2147483648[ms] = 2147483[s] = 596[h] = 24[day]が上限値となっている(int32_tなので…)
         *
         *  \param[in] msec in [ms]
         *  \param[in] repeate true: ret is true, keep flag start state false: ret is true reset flag 
         *  \retval true equal to or more than, mean "Expired"
         *  \retval false less than
         */
        bool isExpire(millisec_interval msec, bool repeate = true) noexcept
        {
            bool ret = false;
            auto now = system_clock::now();
            if (m_flag) {
                ret = (std::chrono::milliseconds(msec) < (now - m_time));
                if (ret && repeate) { // repeate mode (every cicle)
                    m_time = now; // update time to now
                    // m_flag no change
                } else if (ret && !repeate) {
                    m_flag = false;
                }
                else {
                    // No action for !ret
                }
            } else {
//                SMLError("====> The timer not started...");
            }
            return ret;
        }
        /*!  functor for isExpire.
         *
         *
         */
        bool operator ()(millisec_interval msec) noexcept {return isExpire(msec, false);}
        constexpr operator bool() const noexcept {return m_flag;}
        /*! Measure start
         * 計測開始
         *  \retval coutに出力可能(開始時刻でマイクロ秒単位)
         */
        auto start_acquire() noexcept
        {
            this->start();
            return std::chrono::duration_cast<std::chrono::microseconds>(m_time.time_since_epoch()).count();
        }
        /*!  aquire time.
         *
         * 起動時刻(start_auire)からこの関数が呼ばれるまでの差分時間(マイクロ秒単位)
         */
        auto acquire() const noexcept
        {
            return std::chrono::duration_cast<std::chrono::microseconds>(system_clock::now() - m_time).count();
        }
    private:
        time_point m_time;   //!< for mark start time stamp
        bool       m_flag;   //!<
    };
    /*!
     * \example measure_time/example.cpp
     */
} // end of namespace Sml


#endif //<-- macro  SML_MEASURE_TIME_Hpp ends here.
