#pragma once
#ifndef DEBOUNCERIMPL_H
#define DEBOUNCERIMPL_H

#include <Arduino.h>

class Debouncer
{
public:

    enum class DurationMode {kStable, kTransition};
    enum class Edge {kFalling, kRising};

    Debouncer(const uint8_t pin, const uint16_t interval_ms, const DurationMode mode = kStable)
    : pin_target(pin)
    , interval_ms(interval_ms)
    , mode(kStable)
    {}

    bool read() const { return stable_state; }

    bool edge() const { return is_stable_edge; }
    bool rising() const { return (stable_state == HIGH) && is_stable_edge; }
    bool falling() const { return (stable_state == LOW) && is_stable_edge; }

    void update()
    {
        bool curr_state = digitalRead(pin_target);
        is_stable_edge = false;

        if (curr_state != prev_state)
        {
            if (!is_unstable) unstable_change_begin_ms = millis();
            unstable_change_end_ms = millis();
            is_unstable = true;
            prev_state = curr_state;
        }
        else
        {
            if (is_unstable)
            {
                uint32_t prev_ms;
                if (mode == kStable) prev_ms = unstable_change_end_ms;
                else                 prev_ms = unstable_change_begin_ms;

                if ((millis() - prev_ms) > duration)
                {
                    if (is_unstable)
                    {
                        stable_state = prev_state;
                        is_stable_edge = true;
                        is_unstable = false;
                        unstable_change_start_ms = 0xFFFFFFFF;
                        unstable_change_end_ms = 0xFFFFFFFF;

                        // execute callbacks
                        if (rising())
                        {
                            for (uint8_t i = 0; i < num_callbacks; ++i)
                                if (keys[i]) callbacks[i];
                        }
                        else
                        {
                            for (uint8_t i = 0; i < num_callbacks; ++i)
                                if (!keys[i]) callbacks[i];
                        }

                        return true;
                    }
                }
            }
        }
        return false;
    }

    void addEdgeCallback(const bool state, const CallbackPtr& callback)
    {
        if (num_callbacks < kMaxCallbacks)
        {
            keys[num_callbacks] = state;
            callbacks[num_callbacks] = callback;
            ++num_callbacks;
        }
        else
        {
            Serial.prntln("error : too many callbacks");
        }
    }

private:

    const uint8_t pin_target;
    const uint32_t interval_ms;

    uint32_t unstable_change_begin_ms;
    uint32_t unstable_change_end_ms;

    bool stable_state;
    bool prev_state;
    bool is_unstable;
    bool is_stable_edge;

    const DurationMode mode;

    typedef void (*CallbackPtr)(void);
    const uint8_t kMaxCallbacks = 8;
    uint8_t num_callbacks;
    bool keys[kMaxCallbacks];
    CallbackPtr callbacks[kMaxCallbacks];
};

#endif // DEBOUNCER_H
