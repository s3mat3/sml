/**
 * \file light.hpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#pragma once

#ifndef LIGHT_Hpp
# define  LIGHT_Hpp
//#define SML_TRACE
# include "debug.hpp"
# include "fsm.hpp"

class LightEvent : public Sml::FsmEvent
{
public:
    static constexpr FsmEvent off   {FsmEvent::stay};
    static constexpr FsmEvent on    {FsmEvent::stay + 1};
    static constexpr FsmEvent flick {FsmEvent::stay + 2};
};

class Light
{
public:
    using event_id = Sml::event_id;
    virtual ~Light() = default;
    virtual auto turn(event_id) noexcept -> void {}
};

class StateON : public Sml::State<Light>
{
public:
    virtual ~StateON() = default;
    auto doActivity() noexcept -> void override
    {
        MSG("^^^^ Light ON ^^^^");
    }
};
class StateOFF : public Sml::State<Light>
{
public:
    virtual ~StateOFF() = default;
    auto doActivity() noexcept -> void override
    {
        MSG("____ Light OFF ____");
    }
};
class StateFlick : public Sml::State<Light>
{
public:
    virtual ~StateFlick() = default;
    auto doActivity() noexcept -> void override
    {
        MSG("^_^_ Light FLICK _^_^");
    }
};
class LightFSM : public Sml::Fsm<Light>
{
public:
    using state = Sml::State<Light>;
    using event_id = Sml::event_id;
    virtual ~LightFSM() = default;
    LightFSM()
        : on {std::make_shared<StateON>()}
        , off {std::make_shared<StateOFF>()}
        , flick {std::make_shared<StateFlick>()}
    {}
    /** Example for simple dispatch .
     */
    auto dispatch(event_id e) noexcept -> void override
    {
        switch (e) {
        case LightEvent::off: off->doActivity(); break;
        case LightEvent::on: on->doActivity(); break;
        case LightEvent::flick:
        default:
            flick->doActivity();break;
        }
    }
private:
    state_ptr on    {nullptr};
    state_ptr off   {nullptr};
    state_ptr flick {nullptr};
};

class LightUnit : public Light
{
public:
    LightUnit() : fsm {std::make_unique<LightFSM>()}
    {
        fsm->dispatch(LightEvent::off);
    }
    virtual ~LightUnit() = default;
    auto turn(event_id e) noexcept -> void override
    {
        fsm->dispatch(e);
    }
private:
    std::unique_ptr<LightFSM> fsm;
};
#endif //<-- macro  LIGHT_Hpp ends here.
