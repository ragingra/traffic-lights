// main.cpp
#include <Arduino.h>

#include "config.h"
#include "traffic_states.h"
#include "button.h"
#include "web_socket.h"
#include "web_server.h"

bool wifiStarted = false;

StateList* currentStates = &normalStartup;

void setup() {
    Serial.begin(115200);

    pinMode(WIFI_SWITCH_PIN, INPUT_PULLUP);

    // bool wifiEnabled = digitalRead(WIFI_SWITCH_PIN) == LOW;
    bool wifiEnabled = digitalRead(WIFI_SWITCH_PIN) == HIGH;

    if (wifiEnabled) {
        initWebServer();
        initWebSocket();

        wifiStarted = true;
    } else {
        Serial.println("WiFi disabled");
    }

    sr.setAllLow();
    setAllRed();

    pinMode(PED_BUTTON_PIN, INPUT_PULLUP);

    currentStates->reset();
    lastUpdateTime = millis();

    button.attachClick(pedButtonPress);
}

void loop() {
    updateState();
    updatePins();

    if (wifiStarted) {
        webSocket.loop();
    }

    button.tick();
}
