#pragma once
#ifndef DEBOUNCERIMPL_H
#define DEBOUNCERIMPL_H

#include <Arduino.h>
#include <util/atomic.h>
#ifdef __AVR__
#include "RingBuffer.h"
#else
#include <functional>
// #include <map>
#include <vector>
#endif

#ifdef TEENSYDUINO
// dirty for teensy stl compile error
// copied from https://github.com/gcc-mirror/
namespace std
{
    void __throw_bad_function_call() { _GLIBCXX_THROW_OR_ABORT(bad_function_call()); }
}
#endif

// hash expansion
// namespace std
// {
//     template <typename T>
//     struct hash
//     {
//         size_t operator() (T x) const noexcept
//         {
//             using type = typename underlying_type<T>::type;
//             return hash<type>{}(static_cast<type>(x));
//         }
//     };
// }


class Debouncer
{
public:
    enum class DurationFrom {STABLE, TRIGGER};
    enum class Edge {FALL, RISE};
    enum class Active {L, H};

private:
#ifdef __AVR__
    typedef void (*CallbackType)(void);
    struct Map { Edge key; CallbackType func; };
    using CallbackQueue = RingBuffer<Map, 2>;
#else
    using CallbackType = std::function<void(void)>;
    struct Map { Edge key; CallbackType func; };
    // TODO: std::multimap couldn't build on teensy...
    // using CallbackQueue = std::multimap<Edge, CallbackType>;
    using CallbackQueue = std::vector<Map>;
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
        bool curr_state = false;
        uint32_t curr_ms = 0;

        // Temporarily disable interrupts to ensure an accurate time stamp for the sample.
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
          curr_state = digitalRead(pin_target);  // No interrupt will occur between here...
          curr_ms = millis();                    // ...and here, ensuring an accurate time stamp for the sample.
        }
        is_stable_edge = false;

        if (curr_state != prev_state)
        {
            if (!is_unstable) unstable_change_begin_ms = curr_ms;
            unstable_change_end_ms = curr_ms;
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

                if ((curr_ms - prev_ms) > duration_ms)
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

                        // TODO: std::multimap couldn't build on teensy...
                        // auto p = callbacks.equal_range((bool)edge);
                        // for (auto it = p.first; it != p.second; ++it) it->second();

                        for (const auto& c : callbacks) if (edge == c.key) c.func();
                    }
                }
            }
        }
    }

    void subscribe(const Edge edge, const CallbackType func)
    {
        // TODO: std::multimap couldn't build on teensy...
        // callbacks.emplace(std::make_pair(edge, func));
        callbacks.push_back(Map({edge, func}));
    }

private:

    const uint8_t pin_target;

    uint32_t duration_ms;
    uint32_t unstable_change_begin_ms;
    uint32_t unstable_change_end_ms;

    bool stable_state;
    bool prev_state;
    bool is_unstable;
    bool is_stable_edge;

    DurationFrom mode;

    CallbackQueue callbacks;
};

#endif // DEBOUNCERIMPL_H
