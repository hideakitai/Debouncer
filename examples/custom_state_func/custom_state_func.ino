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

    // set your own state function which returns bool with no args instead of `digitalRead(pin)`
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
