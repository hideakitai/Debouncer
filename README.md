# Debouncer
Debounce library for Arduino

## Usage

### Callbacks

``` C++
#include <Debouncer.h>

int pin = 2;
int duration_ms = 50;

Debouncer debouncer(pin, duration_ms);

void setup()
{
    // add from lambda
    debouncer.subscribe(Debouncer::Edge::FALL, [](){
        // do something on falling edge
    });
    debouncer.subscribe(Debouncer::Edge::RISE, [](){
        // do something on rising edge
    });
}

void loop()
{
    debouncer.update();
}
```

#### Limitation for AVR boards (like Uno and Mega)

AVR boards can have only two callbacks. (see `examples/callbacks_uno_avr`)


### Other APIs

``` C++
    debouncer.update();

    Serial.print("current stable state = ");
    Serial.println(debouncer.read());

    if (debouncer.edge())
    {
        if (debouncer.rising())
        {
            Serial.print("rise");
        }
        if (debouncer.falling())
        {
            Serial.print("fall");
        }
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

## Embedded Libraries

- [ArxTypeTraits v0.2.0](https://github.com/hideakitai/ArxTypeTraits)
- [ArxContainer v0.3.9](https://github.com/hideakitai/ArxContainer)
- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)


## LICENSE

MIT
