// button.cpp
#include "button.h"
#include "config.h"
#include <ShiftRegister74HC595.h>
#include <OneButton.h>

#include "traffic_states.h"

ShiftRegister74HC595<2> sr(D8, D7, D6);
OneButton button(PED_BUTTON_PIN, true, true);

void pedButtonPress() {
    pedestrianRequest = true;
    // sr.set(PED_WAIT_PIN, HIGH);
    // bi-leds giving yellow through red and green together
    sr.set({PED_CROSS_PIN}, true);
    sr.set({PED_DONT_CROSS_PIN}, true);
}
