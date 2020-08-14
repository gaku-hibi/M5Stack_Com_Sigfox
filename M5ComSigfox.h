/*
 *  Library of communication between M5Stack and COM.Sigfox 
 *  
 *  Development kit: M5Stack COM.Sigfox
 *
 *  Author: KCCS 
 *  *******************************************************
 *  Serial  -> UART over USB
 *  Serial2 -> TX/RX between M5Stack and COM.Sigfox
 */

#ifndef __M5COMSIGFOX__
#define __M5COMSIGFOX__

#include "ComBufferQueue.h"

class M5ComSigfox : public ComBufferQueue
{
public:
	/* Constructor */
	M5ComSigfox()
	{
		public_key  = false;	// applied private key
		log_serial  = true;		// enable serial log
	};

	/* Destructor */
	~M5ComSigfox(){};
	
	/* Initialize */
	bool begin()
	{
		return init();
	}
	bool begin(bool public_key_usage)
	{
		public_key = public_key_usage;
		return init();
	}

	/* System commands */
	int echo(void);
	int sleep(void);
	int wakeup(void);

	/* Sigfox commands */
	int getId(void);
	int getPac(void);
	int getTemperature(void);
	int getVoltage(void);
	int publicKey(bool enable);

	/* Uplink ASCII string data */
	int uplink(const String data_str);

	/* Uplink hexadecimal ASCII data */
	int uplink(const char *data_str);

	/* Uplink hexadecimal raw data */
	int uplink(const byte *hex_data, int data_len);

	/* Upink ASCII string data for downlink */
	int downlink(const String data_str);

	/* Upink hexadecimal ASCII data for downlink */
	int downlink(const char *data_str);

	/* Upink hexadecimal raw data for downlink */
	int downlink(const byte *hex_data, int data_len);

	/* Data is available in buffer queue */
	bool checkData_Available(void);

	/* Receive function without blocking */
	bool getData_try(char *response, int *resp_len);

	/* Receive function with timeout. If timeout is zero, return immediately (non-blocking) */
	bool getData(char *response, int *resp_len, unsigned int timeout);

	/* Receive and check response is "OK" */
	bool getData_CheckOk(unsigned int timeout);

	/* Configuration of log on Serial (over USB) port */
	void logSerialEnable(bool enable);
	
private:
	/* Initialize */
	bool init(void);

	/* UART to queue */
	void uart_write_queue(void);

	/* Send Sigfox Command */
	int send_command(const char* command);
	
	/* Check command sequence */
	int chk_command_seq(void);

	/* Send frame */
	int sf_send_frame(bool f_downlink, const char* hex_data, int data_len);
	int sf_send_frame(bool f_downlink, const char* data_str);

	/* Public or private key */
	bool public_key = false;
	bool public_key_sent = false;

	/* Flag for module sleep status */
	bool fSleep = true;

	/* Debug log */
	bool log_serial;
	bool checkSerialLog(void);

};

/* status */
#define   NO_ERROR                  (0)
#define   ERR_SERIAL_CLOSED         (1)
#define   ERR_INVALID_PARAMETER     (2)

#endif //__M5COMSIGFOX__
