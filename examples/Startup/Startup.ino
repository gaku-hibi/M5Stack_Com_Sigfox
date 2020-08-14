#include <M5Stack.h>
#include "M5ComSigfox.h"

/* Options */
#define  PUBLIC_KEY_USAGE       (false)     // set 'true' to enable public key (Emulator)
#define  LOG_MESSAGE_DISPLAY    (true)      // set 'true' to enable displaying all message on serial port

/* Definitions */
#define  TIME_WAIT_RESPONSE (1000)          // timeout 1 seconds

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
  char id[12], pac[20], voltage[6], temp[6];
  int  resp_len;
  
  // Get SigFox device ID
  sigfox.getId();
  sigfox.getData(id, &resp_len, TIME_WAIT_RESPONSE);
  
  // Get SigFox PAC 
  sigfox.getPac();
  sigfox.getData(pac, &resp_len, TIME_WAIT_RESPONSE);

  // Get Module Input Voltage
  sigfox.getVoltage();
  sigfox.getData(voltage, &resp_len, TIME_WAIT_RESPONSE);

  // Get Temperature
  sigfox.getTemperature();
  sigfox.getData(temp, &resp_len, TIME_WAIT_RESPONSE);
  float t = (float)atoi(temp) / 10;
  sprintf(temp, "%d.%01d", (int)t, (int)(t*10)%10);
    
  // Print informations
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,0,2);
  M5.Lcd.print("\r\n");
  M5.Lcd.print(" Device ID  : ");
  M5.Lcd.println((const char*) id);
  M5.Lcd.print(" PAC        : ");
  M5.Lcd.println((const char*) pac);
  M5.Lcd.print(" Voltage    : ");
  M5.Lcd.println((const char*)voltage);
  M5.Lcd.print(" Temp       : ");
  M5.Lcd.println((const char*)temp);
  
  // delay 10 seconds
  delay(10000);
}

//Expected Output
//AT$I=10
//00xxxxxx <-Your Device ID
//AT$I=11
//xxxxxxxxxxxxxxxx <-Your Device PAC
//AT$V?
//3280
//AT$T?
//291