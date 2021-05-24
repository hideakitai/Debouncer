#pragma once
#ifndef DEBOUNCERIMPL_H
#define DEBOUNCERIMPL_H

#include "Debouncer/util/ArxTypeTraits/ArxTypeTraits.h"
#include "Debouncer/util/ArxContainer/ArxContainer.h"
#include "Debouncer/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"

class Debouncer {
public:
    enum class DurationFrom {
        STABLE,
        TRIGGER
    };
    enum class Edge {
        FALL,
        RISE,
        CHANGED
    };
    enum class Active {
        L,
        H
    };

private:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
    using StateFunc = std::function<int(void)>;
    using CallbackType = std::function<void(const int)>;
    struct Map {
        Edge key;
        CallbackType func;
    };
    using CallbackQueue = std::vector<Map>;
#else
    typedef int (*StateFunc)(void);
    typedef void (*CallbackType)(const int);
    struct Map {
        Edge key;
        CallbackType func;
    };
    using CallbackQueue = arx::vector<Map>;
#endif

    uint8_t pin_target;
    uint32_t duration_on_ms;
    uint32_t duration_off_ms;
    uint32_t duration_ms;

    uint32_t unstable_change_begin_ms;
    uint32_t unstable_change_end_ms;

    Active active_state;
    int stable_state;
    int prev_state;
    bool is_unstable;
    bool is_stable_edge;

    const DurationFrom mode;

    CallbackQueue callbacks;
    StateFunc state_func {nullptr};

public:
    Debouncer(const uint8_t pin, const uint32_t duration_on_ms, const uint32_t duration_off_ms, const Active active_state = Active::L, const DurationFrom mode = DurationFrom::STABLE)
    : pin_target(pin)
    , duration_on_ms(duration_on_ms)
    , duration_off_ms(duration_off_ms)
    , duration_ms(duration_on_ms)
    , unstable_change_begin_ms(0xFFFFFFFF)
    , unstable_change_end_ms(0xFFFFFFFF)
    , active_state(active_state)
    , stable_state(!(bool)active_state)
    , prev_state(!(bool)active_state)
    , is_unstable(false)
    , is_stable_edge(false)
    , mode(mode) {}

    Debouncer(const uint8_t pin, const uint32_t duration_ms, const Active active_state = Active::L, const DurationFrom mode = DurationFrom::STABLE)
    : pin_target(pin)
    , duration_on_ms(duration_ms)
    , duration_off_ms(duration_ms)
    , duration_ms(duration_ms)
    , unstable_change_begin_ms(0xFFFFFFFF)
    , unstable_change_end_ms(0xFFFFFFFF)
    , active_state(active_state)
    , stable_state(!(bool)active_state)
    , prev_state(!(bool)active_state)
    , is_unstable(false)
    , is_stable_edge(false)
    , mode(mode) {}

    Debouncer(const uint32_t duration_ms, const DurationFrom mode = DurationFrom::STABLE)
    : pin_target(0xFF)
    , duration_on_ms(duration_ms)
    , duration_off_ms(duration_ms)
    , duration_ms(duration_ms)
    , unstable_change_begin_ms(0xFFFFFFFF)
    , unstable_change_end_ms(0xFFFFFFFF)
    , active_state(Active::H)
    , stable_state(0)
    , prev_state(0)
    , is_unstable(false)
    , is_stable_edge(false)
    , mode(mode) {}

    void duration(const uint32_t ms) { duration_ms = ms; }
    uint32_t duration() const { return duration_ms; }

    void stateFunc(const StateFunc& func) {
        pin_target = 0xFF;
        state_func = func;
        stable_state = prev_state = 0;
    }

    void stateFunc(const int default_value, const StateFunc& func) {
        pin_target = 0xFF;
        state_func = func;
        stable_state = prev_state = default_value;
    }

    int read() const { return stable_state; }

    bool edge() const { return is_stable_edge; }
    bool rising() const { return (pin_target != 0xFF) && (stable_state == HIGH) && is_stable_edge; }
    bool falling() const { return (pin_target != 0xFF) && (stable_state == LOW) && is_stable_edge; }
    bool changed() const { return edge(); }

    void update() {
        const uint32_t now = millis();
        int curr_state {0};
        if (pin_target != 0xFF) {
            curr_state = (int)digitalRead(pin_target);
            duration_ms = (stable_state == (bool)active_state) ? duration_off_ms : duration_on_ms;
        } else if (state_func)
            curr_state = state_func();
        else
            Serial.println("Edge callback is not registered. Please register callback first");

        detectEdge(curr_state, now);
    }

    void subscribe(const Edge edge, const CallbackType& func) {
        callbacks.push_back(Map({edge, func}));
    }

    void subscribe(const CallbackType& func) {
        callbacks.push_back(Map({Edge::CHANGED, func}));
    }

private:
    void detectEdge(const int curr_state, const uint32_t now) {
        is_stable_edge = false;

        if (curr_state != prev_state) {
            if (!is_unstable) unstable_change_begin_ms = now;
            unstable_change_end_ms = now;
            is_unstable = true;
            prev_state = curr_state;
        } else {
            if (is_unstable) {
                uint32_t prev_ms;
                if (mode == DurationFrom::STABLE)
                    prev_ms = unstable_change_end_ms;
                else
                    prev_ms = unstable_change_begin_ms;

                if ((now - prev_ms) > duration_ms) {
                    if (is_unstable) {
                        stable_state = prev_state;
                        is_stable_edge = true;
                        is_unstable = false;
                        unstable_change_begin_ms = 0xFFFFFFFF;
                        unstable_change_end_ms = 0xFFFFFFFF;

                        // execute callbacks
                        Edge edge;
                        if (rising())
                            edge = Edge::RISE;
                        else
                            edge = Edge::FALL;

                        for (auto& c : callbacks)
                            if ((c.key == Edge::CHANGED) || (edge == c.key))
                                c.func(stable_state);
                    }
                }
            }
        }
    }
};

#endif  // DEBOUNCERIMPL_H
