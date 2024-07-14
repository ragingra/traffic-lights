#include <Arduino.h>
#include <OneButton.h>
#include <ShiftRegister74HC595.h>
#include <vector>
#include <functional>


#define BUTTON_PIN D3

OneButton button = OneButton(
  BUTTON_PIN,
  true,
  true
);

// Define shift register with 2 74HC595 chips
ShiftRegister74HC595<2> sr(D8, D7, D6);

// Traffic light pins
const int L1_GREEN_PIN = 1, L1_AMBER_PIN = 2, L1_RED_PIN = 3, L1_FILTER_PIN = 4;
const int L2_GREEN_PIN = 5, L2_AMBER_PIN = 6, L2_RED_PIN = 7;
const int L3_GREEN_PIN = 9, L3_AMBER_PIN = 10, L3_RED_PIN = 11, L3_FILTER_PIN = 12;
const int PED_WAIT_PIN = 13, PED_CROSS_PIN = 14, PED_DONT_CROSS_PIN = 15;
const int PED_BUTTON_PIN = 16;  // Pedestrian button pin
const int BEEP_PIN = D2;  // Beep pin

// Constants
const unsigned long FLASH_RATE = 500;
unsigned long lastFlashUpdateTime = 0;
const unsigned long BEEP_RATE = 200;
unsigned long lastBeepUpdateTime = 0;
unsigned long lastUpdateTime = 0;
bool pedestrianRequest = false;

struct TrafficState {
    std::vector<int> activatePins;
    std::vector<int> deactivatePins;
    std::vector<int> flashPins;
    unsigned long duration;
    bool beep;
};

struct StateList {
    std::vector<TrafficState> states;
    std::function<void()> endOfStatesFunc;
    int stateIndex = 0;

    void reset() {
        stateIndex = 0;
    }

    TrafficState& getCurrentState() {
        return states[stateIndex];
    }
};

void pedButtonPress()
{
  pedestrianRequest = true;
  sr.set({PED_WAIT_PIN}, true);
} 

// Forward declaration of state lists and end-of-states functions
void checkForPedestrianRequest();
void switchToNormalStartupSequence();
void switchToPedestrianStartupSequence();
void chooseFilterOrNonFilterSequence();
void switchToFilterSequence();
void switchToNonFilterSequence();
void switchToEndSequence();

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

// Pointer to current StateList
StateList* currentStates = &normalStartup;

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
    if (randomNum <= 4) {
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

    TrafficState state = currentStates->getCurrentState();

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

    // Flash pins
    if (currentTime - lastBeepUpdateTime >= BEEP_RATE) {
        lastBeepUpdateTime = currentTime;
        if (state.beep) {
            int currentState = digitalRead(BEEP_PIN);
            digitalWrite(BEEP_PIN, !currentState);
        }
    }
}

void setAllRed() {
    sr.set(L1_RED_PIN, HIGH);
    sr.set(L2_RED_PIN, HIGH);
    sr.set(L3_RED_PIN, HIGH);
    sr.set(PED_DONT_CROSS_PIN, HIGH);

    // sr.set(0, HIGH);
}

void setup() {
    Serial.begin(115200);
    sr.setAllLow();
    setAllRed();

    pinMode(PED_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BEEP_PIN, OUTPUT);

    // Initialize the current state list
    currentStates->reset();
    lastUpdateTime = millis();

    button.attachClick(pedButtonPress);
}

void loop() {
    button.tick();

    updateState();
}
