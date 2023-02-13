#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_SleepyDog.h>

// Project Includes
#include "Version.h"
#include <pinout.h>

#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma

// LED Strip/String
#define NUM_PIXELS 18

// String of Adafruit WS2801 30mm Pixel Dots
Adafruit_WS2801 strip = Adafruit_WS2801(NUM_PIXELS, PIN_LED_DATA, PIN_LED_CLOCK);

// Serial Link to main pig microcontroller, RX on micro -> TX on RS485 adapter
SoftwareSerial link(PIN_SERIAL_LINK_RX, PIN_SERIAL_LINK_TX);
// Serial communication delimiters
const char startOfNumberDelimiter = '<';
const char endOfNumberDelimiter   = '>';


// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

void setDotDisplay(int num_dots)
{
  for (int i=0; i<NUM_PIXELS; i++) {
    strip.setPixelColor(i, Color(0,0,0));       
  }
  for (int j=0; j<num_dots; j++) {
    strip.setPixelColor(j, Color(0,255,0));
  }
  strip.show();
}

void processNumber (const long n)
  {
  Serial.println (n);
  if((0 < n) && (n < (NUM_PIXELS + 1)))
  {
    setDotDisplay(n);
  }
  }  // end of processNumber
  
void processInput ()
  {
  static long receivedNumber = 0;
  static boolean negative = false;
  
  byte c = link.read ();
  switch (c)
    {
      
    case endOfNumberDelimiter:  
      if (negative) 
        processNumber (- receivedNumber); 
      else
        processNumber (receivedNumber); 

    // fall through to start a new number
    case startOfNumberDelimiter: 
      receivedNumber = 0; 
      negative = false;
      break;
      
    case '0' ... '9': 
      receivedNumber *= 10;
      receivedNumber += c - '0';
      break;
      
    case '-':
      negative = true;
      break;
      
    } // end of switch  
  }  // end of processInput


// ███████╗███████╗████████╗██╗   ██╗██████╗
// ██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
// ███████╗█████╗     ██║   ██║   ██║██████╔╝
// ╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝
// ███████║███████╗   ██║   ╚██████╔╝██║
// ╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝

void setup()
{
  Serial.begin(115200);
  // while (!Serial)
  // {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }
  //Serial.printf("\nProject version v%s, built %s\n", VERSION, BUILD_TIMESTAMP);
  
  Serial.println("Setup function commencing...");

  strip.begin();
  strip.show();

  // Watchdog.enable(4000);

  link.begin(9600);

  Serial.println("Setup Complete");
}

// ██╗      ██████╗  ██████╗ ██████╗
// ██║     ██╔═══██╗██╔═══██╗██╔══██╗
// ██║     ██║   ██║██║   ██║██████╔╝
// ██║     ██║   ██║██║   ██║██╔═══╝
// ███████╗╚██████╔╝╚██████╔╝██║
// ╚══════╝ ╚═════╝  ╚═════╝ ╚═╝

void loop()
{
  while (link.available ())
  {
    processInput();
  }

  // Watchdog.reset();
} // END LOOP