/**
 * @file signal_tower.hpp
 *
 * @copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * @brief
 *
 * @author s3mat3
 */

#pragma once

#ifndef SIGNAL_TOWER_Hpp
# define  SIGNAL_TOWER_Hpp

#include "measure_time.hpp"
#include "light.hpp"

class SignalEvent : public Sml::FsmEvent
{
public:
    static constexpr FsmEvent green  {FsmEvent::stay + 1};
    static constexpr FsmEvent yellow {FsmEvent::stay + 2};
    static constexpr FsmEvent red    {FsmEvent::stay + 3};
    static constexpr FsmEvent broken {FsmEvent::stay + 4};
};

class SignalTower
{
public:
    using event_id = Sml::event_id;
    using fsm_ptr = std::unique_ptr<LightFSM>();
    SignalTower()
        : m_green {}
        , m_yellow {}
        , m_red {}
        , m_event {SignalEvent::void_event}
        , m_count {0}
    {
        auto i = SignalEvent::green;
        SML_LOG(VAR_DUMP(i));
    }
    auto turnRed(event_id e) noexcept {m_red.turn(e);}
    auto turnYellow(event_id e) noexcept {m_yellow.turn(e);}
    auto turnGreen(event_id e) noexcept {m_green.turn(e);}
    auto event(event_id e) noexcept {m_event = e;}
    auto event() const noexcept {return m_event;}
    auto counter() const noexcept -> bool {if (m_count == 10) return false; else return true;}
    auto count() noexcept {++m_count;}
private:
    LightUnit m_green  {};
    LightUnit m_yellow {};
    LightUnit m_red    {};
    Sml::event_id m_event {SignalEvent::void_event};
    Sml::index_type m_count {0};
};

class idle_state : public Sml::State<SignalTower>
{
public:
    using state_type::state_type;
    auto entry() noexcept -> void override {MSG("Entry idle");}
    auto exit() noexcept -> void override {m_context->event(SignalEvent::green);}
};
class green_state : public Sml::State<SignalTower>
{
public:
    using state_type::state_type;
    auto entry() noexcept -> void override
    {
        m_context->turnGreen(LightEvent::on);
        MSG("GREEN");
        m_timer.start(); // timer start
    }
    auto doActivity() noexcept -> void override
    {
        if (m_timer.isExpire(3000, false)) {
            m_context->event(SignalEvent::yellow);
        } else {
            m_context->event(SignalEvent::stay);
        }
    }
    auto exit() noexcept -> void override
    {
        m_context->turnGreen(LightEvent::off);
    }
private:
    Sml::MeasureTime m_timer;
};
/*!  .
 */
class yellow_state : public Sml::State<SignalTower>
{
public:
    using state_type::state_type;
    auto entry() noexcept -> void override
    {
        m_context->turnYellow(LightEvent::on);
        MSG("YELLOW");
        m_timer.start(); // timer start
    }
    auto doActivity() noexcept -> void override
    {
        if (m_timer.isExpire(1000, false)) {
            m_context->event(SignalEvent::red);
        } else {
            m_context->event(SignalEvent::stay);
        }
    }
    auto exit() noexcept -> void override
    {
        m_context->turnYellow(LightEvent::off);
    }
private:
    Sml::MeasureTime m_timer;
};
/*!  .
 */
class red_state : public Sml::State<SignalTower>
{
public:
    using state_type::state_type;
    auto entry() noexcept -> void override
    {
        m_context->turnRed(LightEvent::on);
        MSG("RED");
        m_timer.start(); // timer start
    }
    auto doActivity() noexcept -> void override
    {
        if (m_timer.isExpire(2000, false)) {
            m_context->event(SignalEvent::green);
        } else {
            m_context->event(SignalEvent::stay);
        }
    }
    auto exit() noexcept -> void override
    {
        m_context->turnRed(LightEvent::off);
        m_context->count();
    }
private:
    Sml::MeasureTime m_timer;
};

class SignalFSM : public Sml::Fsm<SignalTower>
{
public:
    SignalFSM(context_ptr context)
        : Sml::Fsm<SignalTower>(context)
        , m_idle_state   {std::make_shared<idle_state>  (1, "idle",   context)}
        , m_green_state  {std::make_shared<green_state> (2, "green",  context)}
        , m_yellow_state {std::make_shared<yellow_state>(3, "yellow", context)}
        , m_red_state    {std::make_shared<red_state>   (4, "red",    context)}
    {
        addTransition(m_idle_state,   SignalEvent::broken, m_idle_state);
        addTransition(m_idle_state,   SignalEvent::green,  m_green_state);
        addTransition(m_idle_state,   SignalEvent::yellow, m_yellow_state);
        addTransition(m_idle_state,   SignalEvent::red,    m_red_state);
        
        addTransition(m_green_state,  SignalEvent::broken, m_idle_state);
        addTransition(m_green_state,  SignalEvent::yellow, m_yellow_state);
        addTransition(m_green_state,  SignalEvent::stay,   m_green_state);
        
        addTransition(m_yellow_state, SignalEvent::broken, m_idle_state);
        addTransition(m_yellow_state, SignalEvent::red,    m_red_state);
        addTransition(m_yellow_state, SignalEvent::stay,   m_yellow_state);
        
        addTransition(m_red_state,    SignalEvent::broken, m_idle_state);
        addTransition(m_red_state,    SignalEvent::green,  m_green_state);
        addTransition(m_red_state,    SignalEvent::stay,   m_red_state);
        m_context->event(SignalEvent::green);
        initial(m_idle_state);
    }
    auto onAnyEvent() noexcept -> void
    {
        while (m_context->counter()) {
            dispatch(m_context->event());
        }
    }
private:
    state_ptr m_idle_state {nullptr};
    state_ptr m_green_state {nullptr};
    state_ptr m_yellow_state {nullptr};
    state_ptr m_red_state {nullptr};
};



#endif //<-- macro  SIGNAL_TOWER_Hpp ends here.
