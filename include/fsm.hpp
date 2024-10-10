/*!
 * \file fsm.hpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Tried to imitate UML's state machine diagram. Nesting (composite state) is not considered
 *
 *  2 main class including
 * - State<context> class  State holder
 * - Fsm<context> class State dispacher and transition building
 *
 *\startuml
 * state State
 *
 * State : entry()
 * State : do() (function doActivity)
 * State : exit()
 *\enduml
 *
 *\code
 * class A : State<context>
 * class B : State<context>
 * class C : State<context>
 * A::m_states -> {1, B}, {2, C}
 *\endcode
 *
 *\startuml
 * state A
 * state B
 * state C
 * A -d-> B : event_id = 1
 * A -d-> C : event_id = 2
 *\enduml
 *
 * \author s3mat3
 */

#pragma once

#ifndef FSM_Hpp
# define  FSM_Hpp
# include <unordered_map>
# include <memory>

# include "base.hpp"
# include "debug.hpp"

namespace Sml {
    using event_id = std::int64_t;
    /*! Event code base class .
     *
     * using stay + n
     */
    class FsmEvent : public code<event_id>
    {
    public:
        FsmEvent() = default;
        constexpr FsmEvent(const event_id id) : code {id} {}
        constexpr FsmEvent(const FsmEvent& id) : code {id} {}
        constexpr FsmEvent(FsmEvent&& id) : code {id} {}
        static constexpr code void_event{INT32_MIN};
        static constexpr code stay{0};
    };
    /*! State class .
     *
     * This class including next states list and context pointer
     * \note nessesary override entry, doActivity, exit functions (Not pure virtual function)
     */
    template <class C>
    class State : public Base
    {
    public:
        using context_type = C;
        using context_ptr  = context_type*;
        using state_type   = State<context_type>;
        using state_ptr    = std::shared_ptr<state_type>;
        using state_wp     = std::weak_ptr<state_type>;
        using states       = std::unordered_map<event_id, state_wp>;
        using Base::Base;
        /*! Default constructor .
         */
        State() = default;//: State(0, "", nullptr) {}
        /*!  Target constructor.
         */
        explicit State(ID_t id, const std::string& name, context_ptr p)
            : Base {id, name}
            , m_context {p}
            , m_states  {}
        {}
        /*! Delive1 constructor .
         */
        explicit State(context_ptr p) : State(0, "", p) {}
        /*! Delive2 constructor .
         */
        explicit State(ID_t id, const std::string& name) : State(id, name, nullptr) {}
        State(const State&)                    = delete;
        State(State&& rhs) noexcept            = delete;
        State& operator=(State&& rhs) noexcept = delete;
        State& operator=(const State&)         = delete;
        virtual ~State()                       = default;
        /*! State entry point .
         */
        virtual auto entry() noexcept -> void {}
        /*! State do .
         */
        virtual auto doActivity() noexcept -> void {}
        /*! State exit .
         */
        virtual auto exit() noexcept -> void {}
        /*! Extract next state by event id .
         *
         *  \param[in] id meaning event id
         *  \retval state_ptr next state
         *  \retval nullptr when catch exception or expired next state pointer
         */
        auto next(event_id id) noexcept -> state_ptr
        {
            state_ptr p = nullptr;
            try {
                if (! m_states.at(id).expired()) {
                    p = m_states.at(id).lock();
                }
            } catch (const std::out_of_range&) {
                SML_FATAL("=====>Not assigned event-code & next state pair");
            } catch (...) {
                SML_FATAL("=====> Occuerred unknown exception");
            }
            return p;
        }
        /*! Add next state by event id .
         * 
         *  \param[in] id is target event id
         *  \param[in] p next state by event_id
         *  \retval void
         */
        auto addNextState(event_id id, state_wp p) noexcept -> void {m_states[id] = p;}

    protected:
        context_ptr m_context {nullptr};
        states      m_states  {};
    };
    /*! Finite state machine .
     *
     * \example fsm/example.cpp
     * \example fsm/light.hpp
     * \example fsm/signal_tower.hpp
     */
    template <class C>
    class Fsm
    {
    public:
        using context_type = C;
        using context_ptr  = context_type*;
        using state        = State<context_type>;
        using state_ptr    = std::shared_ptr<state>;

        Fsm() = default;
        Fsm(context_ptr context) : Fsm(context, nullptr) {}
        Fsm(state_ptr initial) : Fsm(nullptr, initial) {}
        Fsm(context_ptr context, state_ptr initial)
            : m_context {context}
            , m_current {initial}
            // , m_prev {nullptr}
        {}
        virtual ~Fsm() = default;
        auto context() const noexcept -> context_ptr {return m_context;}
        auto context(context_ptr context) noexcept -> void {m_context = context;}
        auto initial(state_ptr i) noexcept {m_current = i;}
        auto addTransition(state_ptr from, event_id e, state_ptr to) const noexcept
        {
            from->addNextState(e, to);
        }
        /*! State dispatcher .
         * This default dispatcher is when current state same previous state and event id equal 0 doActivity only repeat.
         * And when current state same previous and id not equal 0 exit current state after mew state.
         *
         *  \param[in] e is event id
         */
        virtual auto dispatch(event_id e) noexcept ->void
        {
            if (!m_current || e < 0) return; // yield
            if (/*(m_current == m_prev) && */e == 0) { // special transition (internal self transition)
                m_current->doActivity();
            } else {
                m_current->exit();
                auto next = m_current->next(e);
                if (next) {
# if defined (SML_DEBUG_FSM)
                    SML_LOG("**Change state** from " + m_current->name() + " to " + next->name() + " by event id = " + std::to_string(e));
# endif
                    // m_prev    = m_current;
                    m_current = std::move(next);
                }
                m_current->entry();
                m_current->doActivity();
            }
        }
    protected:
        context_ptr m_context {nullptr};
        state_ptr   m_current {nullptr};
        // state_ptr   m_prev    {nullptr};
    };

    /*!
     * \include fsm/signal_tower.hpp
     * \include fsm/light.hpp
     * \include fsm/example.hpp
     */
} //<-- namespace Sml ends here.

#endif //<-- macro  FSM_Hpp ends here.
