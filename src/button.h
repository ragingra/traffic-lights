// button.h
#ifndef BUTTON_H
#define BUTTON_H

#include <ShiftRegister74HC595.h>
#include <OneButton.h>

void pedButtonPress();

extern ShiftRegister74HC595<2> sr;
extern OneButton button;

#endif // BUTTON_H