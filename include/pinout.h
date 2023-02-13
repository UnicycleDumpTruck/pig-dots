#ifndef PINOUT_H
#define PINOUT_H

#include <Arduino.h>


// Button Pins
#define LED_PIN 13

// WS2801 30mm Dot String
#define PIN_LED_DATA    9   // Yellow wire on Adafruit Pixels
#define PIN_LED_CLOCK   8    // Green wire on Adafruit Pixels

// Serial1 link to main pig microcontroller
// NOTE: RX from here is connected to TX on the RS-485 adapter!
#define PIN_SERIAL_LINK_RX 4
#define PIN_SERIAL_LINK_TX 5

#endif
