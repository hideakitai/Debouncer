#include <Debouncer.h>

int pin = 2;
int debounce_duration_ms = 50;

int rise_count = 0;
int fall_count = 0;
int change_count = 0;

// default
Debouncer debouncer(pin, debounce_duration_ms);
// set active HIGH (switch off = LOW)
// Debouncer debouncer(pin, debounce_duration_ms, Debouncer::Active::H);
// set duraion from TRIGGER timing
// Debouncer debouncer(pin, debounce_duration_ms, Debouncer::Active::L, Debouncer::DurationFrom::TRIGGER);

void onRise() { Serial.println("onRise"); }
void onFall() { Serial.println("onFall"); }
void onChanged() { Serial.println("onChanged"); }

void setup()
{
    Serial.begin(115200);
    pinMode(pin, INPUT_PULLUP);

    delay(2000);

    // add from lambda
    debouncer.subscribe(Debouncer::Edge::FALL, [](){
        Serial.print("fall : ");
        Serial.println(fall_count++);
    });
    debouncer.subscribe(Debouncer::Edge::RISE, [](){
        Serial.print("rise : ");
        Serial.println(rise_count++);
    });
    debouncer.subscribe(Debouncer::Edge::CHANGED, [](){
        Serial.print("changed : ");
        Serial.println(change_count++);
    });

    // also you can add callback like this
    debouncer.subscribe(Debouncer::Edge::FALL, onFall);
    debouncer.subscribe(Debouncer::Edge::RISE, onRise);
    debouncer.subscribe(Debouncer::Edge::CHANGED, onChanged);

    Serial.println("start");
}

void loop()
{
    debouncer.update();
}
