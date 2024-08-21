// config.h
#include <Arduino.h>

#ifndef CONFIG_H
#define CONFIG_H

const int L1_GREEN_PIN = 1, L1_AMBER_PIN = 2, L1_RED_PIN = 3, L1_FILTER_PIN = 4;
const int L2_GREEN_PIN = 5, L2_AMBER_PIN = 6, L2_RED_PIN = 7;
const int L3_GREEN_PIN = 9, L3_AMBER_PIN = 10, L3_RED_PIN = 11, L3_FILTER_PIN = 12;
const int PED_WAIT_PIN = 13, PED_CROSS_PIN = 14, PED_DONT_CROSS_PIN = 15;
const int BEEP_PIN = 8;

const int WIFI_SWITCH_PIN = D2;
const int PED_BUTTON_PIN = D3;

const unsigned long FLASH_RATE = 500;
const unsigned long BEEP_RATE = 200;

#endif // CONFIG_H
