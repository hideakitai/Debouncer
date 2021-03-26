# Debouncer

Debounce library for Arduino

- configurabale debounce duration, active low/high, and trigger origin
- register callbacks to `Edge::FALL`, `Edge::RISE`, and `Edge::CHANGED`
- custom state function (you can use `bool foo(void)` to detect state instead of `digitalRead(pin)`)

## Usage

### Simple Usage with Callbacks

``` C++
#include <Debouncer.h>

int pin = 2;
int duration_ms = 50;

// default
Debouncer debouncer(pin, duration_ms);
// set active HIGH (switch off = LOW)
// Debouncer debouncer(pin, debounce_duration_ms, Debouncer::Active::H);
// set duraion from TRIGGER timing
// Debouncer debouncer(pin, debounce_duration_ms, Debouncer::Active::L, Debouncer::DurationFrom::TRIGGER);

void setup()
{
    // add from lambda
    debouncer.subscribe(Debouncer::Edge::FALL, [](){
        // do something on falling edge
    });
    debouncer.subscribe(Debouncer::Edge::RISE, [](){
        // do something on rising edge
    });
    debouncer.subscribe(Debouncer::Edge::CHANGED, [](){
        // do something every edge
    });
}

void loop()
{
    debouncer.update();
}
```

### Manual Operation

``` C++
void loop()
{
    debouncer.update(); // you should update debouncer first

    Serial.print("current stable state = ");
    Serial.println(debouncer.read());

    if (debouncer.edge()) // if edge is detected
    {
        if (debouncer.rising()) // if edge is rising
        {
            Serial.print("rise");
        }
        if (debouncer.falling()) // if edge is falling
        {
            Serial.print("fall");
        }
        if (debouncer.changed()) // if edge is changed
        {
            Serial.print("changed");
        }
    }
}
```

### Custom State Function

```C++
#include <Debouncer.h>

int debounce_duration_ms = 50;
Debouncer debouncer(debounce_duration_ms);

// you can use any state function which returns bool (and no args)
bool customStateFunc() {
    bool b = analogRead(A4);
    return b;
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    // set your own state function which returns bool with no args
    debouncer.stateFunc(customStateFunc);

    // add from lambda
    debouncer.subscribe(Debouncer::Edge::FALL, []() {
        Serial.println("fall");
    });
    debouncer.subscribe(Debouncer::Edge::RISE, []() {
        Serial.println("rise");
    });
    debouncer.subscribe(Debouncer::Edge::CHANGED, []() {
        Serial.println("changed");
    });

    Serial.println("start");
}

void loop() {
    debouncer.update();
}
```

### Active Low / High

``` C++
Debouncer debouncer(pin, duration); // default is active low (switch off = high)
Debouncer debouncer(pin, duration, Debouncer::Active::H); // active high (switch off = low)
```

### Debounce Mode

``` C++
Debouncer debouncer(pin, duration); // check duration after signel becomes stable (default)
Debouncer debouncer(pin, duration, Debouncer::Active::L, Debouncer::DurationFrom::TRIGGER); // check duration from first TRIGGER
```

#### Limitation for AVR boards (like Uno and Mega)

AVR boards can have only two callbacks. (see `examples/callbacks_uno_avr`)


## APIs

``` C++
Debouncer(const uint8_t pin, const uint16_t duration_ms, const Active active = Active::L, const DurationFrom mode = DurationFrom::STABLE);
Debouncer(const uint32_t duration_ms, const Active active = Active::L, const DurationFrom mode = DurationFrom::STABLE);

void duration(const uint32_t ms);
uint32_t duration() const;

void stateFunc(const StateFunc& func);

bool read() const;
void update();

bool edge() const;
bool rising() const;
bool falling() const;
bool changed() const;

void subscribe(const Edge edge, const CallbackType& func);
```

## Parameters

```C++
enum class DurationFrom {STABLE, TRIGGER};
enum class Edge {FALL, RISE, CHANGED};
enum class Active {L, H};
```



## Embedded Libraries

- [ArxTypeTraits v0.2.1](https://github.com/hideakitai/ArxTypeTraits)
- [ArxContainer v0.3.10](https://github.com/hideakitai/ArxContainer)
- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)


## LICENSE

MIT
