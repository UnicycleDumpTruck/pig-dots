#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_SleepyDog.h>

// Project Includes
#include "Version.h"
#include <pinout.h>

#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma

#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>

#define SS_SWITCH        24
#define SS_NEOPIX        6
#define SEESAW_ADDR          0x36

Adafruit_seesaw ss;
seesaw_NeoPixel sspixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);

int32_t encoder_position;

// LED Strip/String
#define NUM_PIXELS 18
uint8_t dataPin  = 9;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 8;    // Green wire on Adafruit Pixels
//uint8_t num_pixels = 18;

// Set the first variable to the NUMBER of pixels.
Adafruit_WS2801 strip = Adafruit_WS2801(NUM_PIXELS, dataPin, clockPin);


SoftwareSerial link(4,5);

// SET A VARIABLE TO STORE THE LED STATE
int ledState = LOW;

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

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
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

  // LED SETUP
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);


  strip.begin();
  strip.show();

  // Encoder setup
  Serial.println("Looking for seesaw!");
  
  if (! ss.begin(SEESAW_ADDR) || ! sspixel.begin(SEESAW_ADDR)) {
    Serial.println("Couldn't find seesaw on default address");
    while(1) delay(10);
  }
  Serial.println("seesaw started");

  uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
  if (version  != 4991){
    Serial.print("Wrong firmware loaded? ");
    Serial.println(version);
    while(1) delay(10);
  }
  Serial.println("Found Product 4991");

  // set not so bright!
  sspixel.setBrightness(20);
  sspixel.show();
  
  // use a pin for the built in encoder switch
  ss.pinMode(SS_SWITCH, INPUT_PULLUP);

  // get starting position
  encoder_position = ss.getEncoderPosition();

  Serial.println("Turning on interrupts");
  delay(10);
  ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
  ss.enableEncoderInterrupt();



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


  if (! ss.digitalRead(SS_SWITCH)) {
    Serial.println("Button pressed!");
  }

  int32_t new_position = ss.getEncoderPosition();
  // did we move around?
  if (encoder_position != new_position) {
    if (new_position > NUM_PIXELS) {
      new_position = NUM_PIXELS;
    }
    if (new_position < 0) {
      new_position = 0;
    }

    setDotDisplay(new_position);

    Serial.println(new_position);         // display new position

    // change the encoder's neopixel color
    // sspixel.setPixelColor(0, Wheel(new_position & 0xFF));
    // sspixel.show();

    encoder_position = new_position;      // and save for next round
    ss.setEncoderPosition(encoder_position);    
  }



  // Watchdog.reset();
} // END LOOP