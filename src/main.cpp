#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_SleepyDog.h>

// Project Includes/dev/ttyUSB0
#include "Version.h"
#include <pinout.h>

SoftwareSerial link(4,5);
// char old_count_text = "";
// char count_text = "";
// how much serial data we expect before a newline
const unsigned int MAX_INPUT = 50;


// SET A VARIABLE TO STORE THE LED STATE
int ledState = LOW;


void process_data (const char * data)
  {
  // for now just display it
  // (but you could compare it to some value, convert to an integer, etc.)
  Serial.println (data);
  }  // end of process_data
  
void processIncomingByte (const byte inByte)
  {
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte)
    {

    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte
      
      // terminator reached! process input_line here ...
      process_data (input_line);
      
      // reset buffer for next time
      input_pos = 0;  
      break;

    case '\r':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;

    }  // end of switch
   
  } // end of processIncomingByte  



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

  // Watchdog.enable(4000);

  link.begin(300);
  delay(2000);
  link.print('b');
  delay(20);
  link.print('b');

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
  while (link.available() == 0) {}
  Serial.println("=========================================================================");
  processIncomingByte (link.read ());
  // byte count = link.read();
  // // int count_int = link.parseInt();
  // // Serial.println(count_int);
  // // if(count > 0)
  // // { 
  // Serial.print("count received: ");
  // Serial.println(count);
  // ledState = !ledState;
  // digitalWrite(LED_PIN, ledState);
  // link.print(count);
  // }


  // Watchdog.reset();
}