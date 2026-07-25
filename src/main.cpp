#include <Arduino.h>
#include "AppController.hpp"

static AppController app;

void setup() {
    app.setup();
}

void loop() {
    app.loop();
}