#include <Debouncer.h>

int pin = 2;
int debounce_duration_on_ms = 3000;
int debounce_duration_off_ms = 50;

int rise_count = 0;
int fall_count = 0;
int change_count = 0;

// default
Debouncer debouncer(pin, debounce_duration_on_ms, debounce_duration_off_ms);
// set active HIGH (switch off = LOW)
// Debouncer debouncer(pin, debounce_duration_ms, Debouncer::Active::H);
// set duraion from TRIGGER timing
// Debouncer debouncer(pin, debounce_duration_ms, Debouncer::Active::L, Debouncer::DurationFrom::TRIGGER);

void setup() {
    Serial.begin(115200);
    pinMode(pin, INPUT_PULLUP);

    delay(2000);

    // add from lambda
    debouncer.subscribe(Debouncer::Edge::FALL, [](const int state) {
        Serial.print("fall (Switch ON = Long Pressed!) : ");
        Serial.println(fall_count++);
    });
    debouncer.subscribe(Debouncer::Edge::RISE, [](const int state) {
        Serial.print("rise (Switch OFF) : ");
        Serial.println(rise_count++);
    });
    debouncer.subscribe(Debouncer::Edge::CHANGED, [](const int state) {
        Serial.print("changed : ");
        Serial.println(change_count++);
    });

    Serial.println("start");
}

void loop() {
    debouncer.update();
}
