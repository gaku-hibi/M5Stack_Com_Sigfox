#include "M5ComSigfox.h"

/* Options */
#define  PUBLIC_KEY_USAGE       (false)         // set 'true' to enable public key (Emulator)
#define  LOG_MESSAGE_DISPLAY    (true)          // set 'true' to enable displaying all message on serial port

M5ComSigfox sigfox;

void setup() {
	Serial.begin(115200);
	sigfox.begin(PUBLIC_KEY_USAGE);
	sigfox.getId();
}

void loop() {

}