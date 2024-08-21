// traffic_states.cpp
#include "traffic_states.h"
#include <ArduinoJson.h>

#include "web_socket.h"
#include "button.h"
#include "config.h"

unsigned long lastFlashUpdateTime = 0;
unsigned long lastBeepUpdateTime = 0;
unsigned long lastUpdateTime = 0;
bool pedestrianRequest = false;

StateList normalStartup = {
    {
        {{L1_AMBER_PIN, L2_AMBER_PIN}, {L1_RED_PIN, L2_RED_PIN}, {}, 3000, false},
        {{L1_GREEN_PIN, L2_GREEN_PIN}, {L1_AMBER_PIN, L2_AMBER_PIN}, {}, 5000, false}
    },
    chooseFilterOrNonFilterSequence
};

StateList PedStartup = {
    {
        {{PED_CROSS_PIN}, {PED_DONT_CROSS_PIN, PED_WAIT_PIN}, {}, 3200, true},
        {{}, {}, {PED_CROSS_PIN}, 2500, false},
        {{}, {L1_RED_PIN, L2_RED_PIN}, {PED_CROSS_PIN, L1_AMBER_PIN, L2_AMBER_PIN}, 3000, false},
        {{L1_GREEN_PIN, L2_GREEN_PIN, PED_DONT_CROSS_PIN}, {L1_AMBER_PIN, L2_AMBER_PIN, PED_CROSS_PIN}, {}, 5000, false}
    },
    chooseFilterOrNonFilterSequence
};

StateList FilterSequence = {
    {
        {{L2_AMBER_PIN}, {L2_GREEN_PIN}, {}, 3000, false},
        {{L2_RED_PIN}, {L2_AMBER_PIN}, {}, 3000, false},
        {{L1_FILTER_PIN, L3_FILTER_PIN}, {}, {}, 3000, false},
        {{L1_AMBER_PIN}, {L1_GREEN_PIN, L1_FILTER_PIN}, {}, 3000, false},
        {{L1_RED_PIN}, {L1_AMBER_PIN}, {}, 3000, false},
        {{L3_AMBER_PIN}, {L3_RED_PIN}, {}, 3000, false},
        {{L3_GREEN_PIN}, {L3_AMBER_PIN}, {}, 3000, false}
    },
    switchToEndSequence
};

StateList NonFilterSequence = {
    {
        {{L1_AMBER_PIN, L2_AMBER_PIN}, {L1_GREEN_PIN, L2_GREEN_PIN}, {}, 3000, false},
        {{L1_RED_PIN, L2_RED_PIN}, {L1_AMBER_PIN, L2_AMBER_PIN}, {}, 3000, false},
        {{L3_AMBER_PIN}, {L3_RED_PIN}, {}, 3000, false},
        {{L3_GREEN_PIN, L3_FILTER_PIN}, {L3_AMBER_PIN}, {}, 5000, false}
    },
    switchToEndSequence
};

StateList EndSequence = {
    {
        {{L3_AMBER_PIN}, {L3_GREEN_PIN, L3_FILTER_PIN}, {}, 3000, false},
        {{L3_RED_PIN}, {L3_AMBER_PIN}, {}, 2000, false}
    },
    checkForPedestrianRequest
};

void checkForPedestrianRequest() {
    if (pedestrianRequest) {
        switchToPedestrianStartupSequence();
    } else {
        switchToNormalStartupSequence();
    }
}

void switchToNormalStartupSequence() {
    currentStates = &normalStartup;
    currentStates->reset();
    lastUpdateTime = millis();
}

void switchToPedestrianStartupSequence() {
    currentStates = &PedStartup;
    pedestrianRequest = false;
    currentStates->reset();
    lastUpdateTime = millis();
}

void switchToFilterSequence() {
    currentStates = &FilterSequence;
    currentStates->reset();
    lastUpdateTime = millis();
}

void switchToNonFilterSequence() {
    currentStates = &NonFilterSequence;
    currentStates->reset();
    lastUpdateTime = millis();
}

void switchToEndSequence() {
    currentStates = &EndSequence;
    currentStates->reset();
    lastUpdateTime = millis();
}

void chooseFilterOrNonFilterSequence() {
    // random number between 0 and 5, then choose filter or non-filter sequence
    int randomNum = random(6);
    if (randomNum <= 3) {
        switchToFilterSequence();
    } else {
        switchToNonFilterSequence();
    }
}

void updateState() {
    unsigned long currentTime = millis();

    // Check if it's time to change the state
    if (currentTime - lastUpdateTime >= currentStates->getCurrentState().duration) {
        lastUpdateTime = currentTime;

        // Move to the next state
        currentStates->stateIndex++;
        if (currentStates->stateIndex >= static_cast<int>(currentStates->states.size())) {
            currentStates->endOfStatesFunc();
        }
    }
}

void updatePins() {
    unsigned long currentTime = millis();
    TrafficState state = currentStates->getCurrentState();


    // declare array called pinsBefore, to then check if the pins have changed
    int pinsBefore[16];
    for (int i = 0; i < 16; i++) {
        pinsBefore[i] = sr.get(i);
    }

    // Activate pins
    for (int pin : state.activatePins) {
        sr.set(pin, HIGH);
    }

    // Deactivate pins
    for (int pin : state.deactivatePins) {
        sr.set(pin, LOW);
    }

    // Flash pins
    if (currentTime - lastFlashUpdateTime >= FLASH_RATE) {
        lastFlashUpdateTime = currentTime;
        for (int pin : state.flashPins) {
            sr.set(pin, !sr.get(pin));
        }
    }

    // Beep
    if (currentTime - lastBeepUpdateTime >= BEEP_RATE) {
        lastBeepUpdateTime = currentTime;
        if (state.beep) {
            
            int currentState = sr.get(BEEP_PIN);
            sr.set(BEEP_PIN, !currentState);
        }
    }

    bool pinsChanged = false;
    for (int i = 0; i < 16; i++) {
        if (pinsBefore[i] != sr.get(i)) {
            pinsChanged = true;
            break;
        }
    }
    
    if (pinsChanged) {
        JsonDocument doc;

        doc["message_type"] = "state_change";

        JsonArray data = doc["pin_data"].to<JsonArray>();
        for (int i = 0; i < 16; i++) {
            data.add(sr.get(i));
        }

        String output;
        serializeJson(doc, output);

        webSocket.broadcastTXT(output);
    }
}

void setAllRed() {
    sr.set(L1_RED_PIN, HIGH);
    sr.set(L2_RED_PIN, HIGH);
    sr.set(L3_RED_PIN, HIGH);
    sr.set(PED_DONT_CROSS_PIN, HIGH);
}
