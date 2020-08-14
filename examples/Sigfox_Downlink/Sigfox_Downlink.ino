#include <M5Stack.h>
#include "M5ComSigfox.h"

/* Options */
#define  PUBLIC_KEY_USAGE       (false)     // set 'true' to enable public key (Emulator)
#define  LOG_MESSAGE_DISPLAY    (true)      // set 'true' to enable displaying all message on serial port

/* Definitions */
#define  TIME_WAIT_DOWNLINK (60000)   // timeout 60 seconds for downlink

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
  char response[128];
  char my_data[32];
  int  resp_len;
  strcpy(my_data, "12345678901234567890ABCD");

  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,0,2);
  M5.Lcd.print("\r\n");
  M5.Lcd.print(" Send Frame: ");
  M5.Lcd.println((const char*)my_data);
  
  sigfox.downlink((const char*)my_data);

  // Wait downlink
  if (sigfox.getData_CheckOk(TIME_WAIT_DOWNLINK)) {
    M5.Lcd.println(" >OK");
  }
  // Get downlink payload
  if (sigfox.getData(response, &resp_len, 500)) {
    if (resp_len) {
      M5.Lcd.print(" Recieve downlink: ");
      M5.Lcd.println((const char*) response);
      M5.Lcd.print(" Length: ");
      M5.Lcd.println(resp_len);
    } else {
      M5.Lcd.println(" Downlink failed...");
    }
  } else {
    M5.Lcd.println(" Timeout downlink...");
  }

  M5.Lcd.println(" Wait 60 seconds...");
  delay(60000);
}

//Expected Output
//AT$SF=12345678901234567890ABCD,1
//OK
//RX=00 00 58 6B 00 00 FF 91
