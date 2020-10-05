#pragma once
#ifndef DEBOUNCERIMPL_H
#define DEBOUNCERIMPL_H

#include "Debouncer/util/ArxTypeTraits/ArxTypeTraits.h"
#include "Debouncer/util/ArxContainer/ArxContainer.h"
#include "Debouncer/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"


class Debouncer
{
public:

    enum class DurationFrom {STABLE, TRIGGER};
    enum class Edge {FALL, RISE};
    enum class Active {L, H};

private:

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L // Have libstdc++11
    using CallbackType = std::function<void(void)>;
    struct Map { Edge key; CallbackType func; };
    using CallbackQueue = std::vector<Map>;
#else
    typedef void (*CallbackType)(void);
    struct Map { Edge key; CallbackType func; };
    using CallbackQueue = arx::vector<Map>;
#endif

public:

    Debouncer(const uint8_t pin, const uint16_t duration_ms, const Active active = Active::L, const DurationFrom mode = DurationFrom::STABLE)
    : pin_target(pin)
    , duration_ms(duration_ms)
    , unstable_change_begin_ms(0xFFFFFFFF)
    , unstable_change_end_ms(0xFFFFFFFF)
    , stable_state(!(bool)active)
    , prev_state(!(bool)active)
    , is_unstable(false)
    , is_stable_edge(false)
    , mode(mode)
    {}

    bool read() const { return stable_state; }

    bool edge() const { return is_stable_edge; }
    bool rising() const { return (stable_state == HIGH) && is_stable_edge; }
    bool falling() const { return (stable_state == LOW) && is_stable_edge; }

    void update()
    {
        const bool curr_state = digitalRead(pin_target);
        is_stable_edge = false;
        const uint32_t now = millis();

        if (curr_state != prev_state)
        {
            if (!is_unstable) unstable_change_begin_ms = now;
            unstable_change_end_ms = now;
            is_unstable = true;
            prev_state = curr_state;
        }
        else
        {
            if (is_unstable)
            {
                uint32_t prev_ms;
                if (mode == DurationFrom::STABLE) prev_ms = unstable_change_end_ms;
                else                              prev_ms = unstable_change_begin_ms;

                if ((now - prev_ms) > duration_ms)
                {
                    if (is_unstable)
                    {
                        stable_state = prev_state;
                        is_stable_edge = true;
                        is_unstable = false;
                        unstable_change_begin_ms = 0xFFFFFFFF;
                        unstable_change_end_ms = 0xFFFFFFFF;

                        // execute callbacks
                        Edge edge;
                        if (rising())  edge = Edge::RISE;
                        else           edge = Edge::FALL;

                        for (auto& c : callbacks) if (edge == c.key) c.func();
                    }
                }
            }
        }
    }

    void subscribe(const Edge edge, const CallbackType& func)
    {
        callbacks.push_back(Map({edge, func}));
    }

private:

    const uint8_t pin_target;
    const uint32_t duration_ms;

    uint32_t unstable_change_begin_ms;
    uint32_t unstable_change_end_ms;

    bool stable_state;
    bool prev_state;
    bool is_unstable;
    bool is_stable_edge;

    const DurationFrom mode;

    CallbackQueue callbacks;
};

#endif // DEBOUNCERIMPL_H
