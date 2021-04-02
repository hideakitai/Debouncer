#include <Debouncer.h>

int pin = 2;
int debounce_duration_ms = 50;

int rise_count = 0;
int fall_count = 0;

Debouncer debouncer(pin, debounce_duration_ms);

void setup() {
    Serial.begin(115200);
    pinMode(pin, INPUT_PULLUP);

    delay(2000);

    Serial.println("start");
}

void loop() {
    debouncer.update();

    // Serial.print("current unstable state = ");
    // Serial.println(digitalRead(pin));

    // Serial.print("current stable state = ");
    // Serial.println(debouncer.read());

    if (debouncer.edge()) {
        Serial.print("edge detect: ");
        if (debouncer.rising()) {
            Serial.print("rise : ");
            Serial.println(rise_count++);
        }
        if (debouncer.falling()) {
            Serial.print("fall : ");
            Serial.println(fall_count++);
        }
    }
}
