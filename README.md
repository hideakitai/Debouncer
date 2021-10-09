# Debouncer

Debounce library for Arduino

- configurabale debounce duration, active low/high, and trigger origin
- register callbacks to `Edge::FALL`, `Edge::RISE`, and `Edge::CHANGED`
- register custom state function to detect edge (instead of `digitalRead(pin)`)
  - you can use `int custom_func(void)` to detect state instead of `digitalRead(pin)`

## Usage

### Simple Usage with Callbacks

```C++
#include <Debouncer.h>

int pin = 2;
int duration_ms = 50;

// default
Debouncer debouncer(pin, duration_ms);
// set active HIGH (switch off = LOW)
// Debouncer debouncer(pin, debounce_duration_ms, Debouncer::Active::H);
// set duraion from TRIGGER timing
// Debouncer debouncer(pin, debounce_duration_ms, Debouncer::Active::L, Debouncer::DurationFrom::TRIGGER);

void setup() {
    // add from lambda
    debouncer.subscribe(Debouncer::Edge::FALL, [](const int state) {
        // do something on falling edge
    });
    debouncer.subscribe(Debouncer::Edge::RISE, [](const int state) {
        // do something on rising edge
    });
    debouncer.subscribe(Debouncer::Edge::CHANGED, [](const int state) {
        // do something every edge
    });
    // this simplified one can also be used to register callback to Edge::CHANGED
    debouncer.subscribe([](const int state) {
        // do something every edge
    });
}

void loop() {
    debouncer.update();
}
```

### Manual Operation

```C++
void loop() {
    debouncer.update(); // you should update debouncer first

    Serial.print("current stable state = ");
    Serial.println(debouncer.read());

    if (debouncer.edge()) {       // if edge is detected
        if (debouncer.rising())   // if edge is rising
            Serial.print("rise");
        if (debouncer.falling())  // if edge is falling
            Serial.print("fall");
        if (debouncer.changed())  // if edge is changed
            Serial.print("changed");
    }
}
```

### Custom State Function

```C++
#include <Debouncer.h>

int debounce_duration_ms = 50;
Debouncer debouncer(debounce_duration_ms); // do not specify pin number

// you can use any state function which returns integer (with no args)
int customStateFunc() {
    int b0 = digitalRead(2);
    int b1 = digitalRead(3);
    int value = b0 | (b1 << 1); // value is 0, 1, 2, 3
    return value;
}

void setup() {
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);

    // you can use any state function which returns int (and no args)
    // instead of `digitalRead(pin)`
    // debouncer.stateFunc([](){
    //     int b0 = digitalRead(2);
    //     int b1 = digitalRead(3);
    //     int value = b0 | (b1 << 1);
    //     return value;
    // });

    // you can also set default value for custom state function
    debouncer.stateFunc(3, []() {
        int b0 = digitalRead(2);
        int b1 = digitalRead(3);
        int value = b0 | (b1 << 1);
        return value;
    });
}

void loop() {
    debouncer.update();
}
```

### Active Low / High

```C++
Debouncer debouncer(pin, duration); // default is active low (switch off = high)
Debouncer debouncer(pin, duration, Debouncer::Active::H); // active high (switch off = low)
void setActiveState(const Active state);
```

### Debounce Mode

```C++
Debouncer debouncer(pin, duration); // check duration after signel becomes stable (default)
Debouncer debouncer(pin, duration, Debouncer::Active::L, Debouncer::DurationFrom::TRIGGER); // check duration from first TRIGGER
void setDurationMode(const DurationFrom m);
```

### Custom State Function (without `digitalRead(pin)`)

```C++
Debouncer debouncer(duration); // without specifying the pin, you can use custom state function
Debouncer debouncer(duration, Debouncer::DurationFrom::TRIGGER); // duration mode can be changed
void stateFunc(const StateFunc& func);
void stateFunc(const int default_value, const StateFunc& func);
```

#### Limitation for AVR boards (like Uno and Mega)

AVR boards can have only two callbacks. (see `examples/callbacks_uno_avr`)

## APIs

```C++
Debouncer();
Debouncer(const uint8_t pin, const uint32_t duration_on_ms, const uint32_t duration_off_ms, const Active active_state = Active::L, const DurationFrom mode = DurationFrom::STABLE);
Debouncer(const uint8_t pin, const uint32_t duration_ms, const Active active = Active::L, const DurationFrom mode = DurationFrom::STABLE);
Debouncer(const uint32_t duration_ms, const DurationFrom mode = DurationFrom::STABLE);

void subscribe(const Edge edge, const CallbackType& func);
void subscribe(const CallbackType& func);

void stateFunc(const StateFunc& func);
void stateFunc(const int default_value, const StateFunc& func);

void duration(const uint32_t ms);
uint32_t duration() const;
int read() const;
bool edge() const;
bool rising() const;
bool falling() const;
bool changed() const;

void update();

void setTargetPin(const uint8_t pin);
void setActiveState(const Active state);
void setDuration(const uint32_t ms);
void setDurationActivate(const uint32_t ms);
void setDurationDeactivate(const uint32_t ms);
void setDurationMode(const DurationFrom m);
void setActive(const bool b);
bool isActive() const;
```

## Parameters

```C++
enum class DurationFrom {STABLE, TRIGGER};
enum class Edge {FALL, RISE, CHANGED};
enum class Active {L, H};
```

## Embedded Libraries

- [ArxTypeTraits v0.2.3](https://github.com/hideakitai/ArxTypeTraits)
- [ArxContainer v0.4.0](https://github.com/hideakitai/ArxContainer)
- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)

## LICENSE

MIT
