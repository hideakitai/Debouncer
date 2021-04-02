#include <Debouncer.h>

int debounce_duration_ms = 50;
Debouncer debouncer(debounce_duration_ms);

void setup() {
    Serial.begin(115200);
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    delay(2000);

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

    // if you set your state function, you need to set your callback to CHANGED
    // if you don't specify the Edge, the callback is set to Edge::CHANGE automatically
    // note that not only Edge::FALL/RISE but also Active::HIGH/LOW are also disabled
    debouncer.subscribe([](const int state) {
        Serial.print("changed to ");
        Serial.println(state);
    });

    Serial.println("start");
}

void loop() {
    debouncer.update();
}
