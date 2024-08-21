// traffic_states.h
#ifndef TRAFFIC_STATES_H
#define TRAFFIC_STATES_H

#include <vector>
#include <functional>

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

extern StateList normalStartup;
extern StateList* currentStates;

extern unsigned long lastUpdateTime;
extern bool pedestrianRequest;

void switchToNormalStartupSequence();
void switchToPedestrianStartupSequence();
void switchToFilterSequence();
void switchToNonFilterSequence();
void switchToEndSequence();
void chooseFilterOrNonFilterSequence();
void updateState();
void updatePins();
void checkForPedestrianRequest();
void setAllRed();

#endif // TRAFFIC_STATES_H
