#include <M5Stack.h>
#include "M5ComSigfox.h"

/* Options */
#define  PUBLIC_KEY_USAGE       (false)     // set 'true' to enable public key (Emulator)
#define  LOG_MESSAGE_DISPLAY    (true)      // set 'true' to enable displaying all message on serial port

/* Definitions */
#define  TIME_WAIT_RESPONSE (1000)          // timeout 1 seconds
#define  TIME_WAIT_UPLINK   (15000)         // timeout 15 seconds for uplink

M5ComSigfox sigfox;

void setup() {
  M5.begin();
  
  // Serial port M5Stack over USB
  Serial.begin(115200);

  // Initialize component
  sigfox.begin(PUBLIC_KEY_USAGE);

  // Enable Serial Log
  sigfox.logSerialEnable(LOG_MESSAGE_DISPLAY);

  delay(5000);
}

void loop() {
  char my_data[32];
  strcpy(my_data, "112233445566778899AABBCC");

  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,0,2);
  M5.Lcd.print("\r\n");
  M5.Lcd.print(" Send Frame: ");
  M5.Lcd.println((const char*)my_data);
  
  sigfox.uplink((const char*)my_data);

  // Wait response
  if (sigfox.getData_CheckOk(TIME_WAIT_UPLINK)) {
    M5.Lcd.println(" >OK");
  }

  M5.Lcd.println(" Wait 60 seconds...");
  delay(60000);
}

//Expected Output
//AT$SF=112233445566778899AABBCC,0
//OK
