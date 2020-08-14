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

#include <M5Stack.h>

#include "M5ComSigfox.h"

/* function for all methods */
/*
void sscanf_02X(const char *src, byte *out)
{
    *out = 0;
    if (src[0] >= '0' && src[0] <= '9')
    {
        *out = (src[0] - '0') << 4;
    }
    else
    {
        *out = (src[0] - 'A' + 10) << 4;
    }
    
    if (src[1] >= '0' && src[1] <= '9')
    {
        *out += (src[1] - '0');
    }
    else
    {
        *out += (src[1] - 'A' + 10);
    }
}*/

/* ***************** UnaMKR ***************** */
bool M5ComSigfox :: init(void)
{
	unsigned long ctime, stime = millis();
	
	// PIO serial port (TX, RX)
	Serial2.begin(9600, SERIAL_8N1, 16, 17);
	
	// Serial log message
	logSerialEnable(true);
	
	// clear buffer queue
	resetQueue();
	return true;
}

/* ***************** Commands ***************** */
int M5ComSigfox :: echo(void)
{
	return send_command("AT?");
}

int M5ComSigfox ::sleep(void)
{
	int r = send_command("AT$P=1");
	if (r == NO_ERROR) {
		fSleep = true;
	}
	return r;
}

int M5ComSigfox :: wakeup(void)
{
    if (!Serial2)
        return ERR_SERIAL_CLOSED;

    Serial2.print("\r\r");
    delay(100);
    while (Serial2.available()) Serial2.read();

    return NO_ERROR;
}

/* Sigfox commands */
int M5ComSigfox :: getId(void)
{
	return send_command("AT$I=10");
}

int M5ComSigfox :: getPac(void)
{
	return send_command("AT$I=11");
}

// get module input volatage
int M5ComSigfox :: getVoltage(void)
{
	return send_command("AT$V?");
}

// get Temperature on Sigfox module
int M5ComSigfox :: getTemperature(void)
{
	return send_command("AT$T?");
}

int M5ComSigfox :: publicKey(bool enable)
{
    char str[16];
    sprintf(str, "ATS410=%d", (enable)? 1 : 0);
	
	return send_command((const char *)str);
}

// send frame data (ASCII string)
int M5ComSigfox :: uplink(const String data_str)
{
    char buf[12];
    int  length = data_str.length();
    if (length <= 12)
    {
        data_str.toCharArray(buf, 12);
        return sf_send_frame(false, (const char*) buf, length);
    }
    else
    {
        return ERR_INVALID_PARAMETER;
    }
}

int M5ComSigfox :: downlink(const String data_str)
{
    char buf[12];
    int  length = data_str.length();
    if (length <= 12)
    {
        data_str.toCharArray(buf, 12);
        return sf_send_frame(true, (const char*) buf, length);
    }
    else
    {
        return ERR_INVALID_PARAMETER;
    }
}


// send frame data (ASCII data)
int M5ComSigfox :: uplink(const char *data_str)
{
    return sf_send_frame(false, data_str);
}
int M5ComSigfox :: downlink(const char *data_str)
{
    return sf_send_frame(true, data_str);
}

// send frame data (HEX)
int M5ComSigfox :: uplink(const byte *hex_data, int data_len)
{
    return sf_send_frame(false, (const char*)hex_data, data_len);
}
int M5ComSigfox :: downlink(const byte *hex_data, int data_len)
{
    return sf_send_frame(true, (const char*)hex_data, data_len);
}

int M5ComSigfox :: sf_send_frame(bool f_downlink, const char* hex_data, int data_len)
{
    char *p, str[64];
    
    int status = chk_command_seq();
    if (status) return status;

    if (data_len > 12)
        return ERR_INVALID_PARAMETER;

    // set private/public key
    if (!public_key_sent)
    {
        bool logprn = log_serial;
        public_key_sent = true;

        logSerialEnable(false);
        publicKey(public_key);
        getData_CheckOk(1000);
        logSerialEnable(logprn);

        delay(1000);
    }
    resetQueue();
    Serial2.flush();
    p = str;
    p += sprintf(p, "AT$SF=");

    for(int i=0; i<data_len; i++)
        p += sprintf(p, "%02X", hex_data[i]);

    *(p ++) = ',';

    if (f_downlink)
        *(p ++) = '1';
    else
        *(p ++) = '0';

    *(p ++) = '\r';
    *(p ++) = '\0';
    
    if (log_serial) Serial.println((const char *) str);
    Serial2.print((const char *) str);
    return NO_ERROR;
}

int M5ComSigfox :: sf_send_frame(bool f_downlink, const char* data_str)
{
    char str[64];
    int  data_len = strlen(data_str)/2;

    int status = chk_command_seq();
    if (status) return status;

    if (data_len > 12)
        return ERR_INVALID_PARAMETER;

    resetQueue();
    Serial2.flush();
    sprintf(str, "AT$SF=%s,%d\r", data_str, (f_downlink) ? 1 : 0);
    
    if (log_serial) Serial.println((const char *) str);
    Serial2.print((const char *) str);
    return NO_ERROR;
}

/* data is available in buffer queue */
bool M5ComSigfox :: checkData_Available(void)
{
    return checkOneLineReceived();
}

/* non-blocked read data from UART to buffer queue*/
bool M5ComSigfox :: getData_try(char *response, int *resp_len)
{
    bool data_in_queued;

    uart_write_queue();

    /* check response data & copy to buffer 'response' */
    data_in_queued = checkOneLineReceived();
    if (response && data_in_queued)
    {
        // check the end of response
        if (checkResponseReceived())
        {
            // end of response '\r'
            if (getResponseData(response, resp_len))
            {
                clearOneLineReceived();
            }
        }
        else
        {
            // one-line
            for (int i=0; i<4; i++)
            {
                char nl = getNewLineCharacter(i);
                if (nl && readOneLine(response, resp_len, nl) == true)
                {
                    clearOneLineReceived();
                    break;
                }
            }
        }
    }
    return data_in_queued;
}

bool M5ComSigfox :: getData(char *response, int *resp_len, unsigned int timeout)
{
    unsigned long delta, ctime, stime, l_timeout;
    bool read_success = false;

    stime = millis();
    l_timeout = (unsigned long) timeout;
    
    do
    {
        read_success = getData_try(response, resp_len);

        // end of string
        if (read_success && response && resp_len) 
        {
            response[*resp_len] = '\0';
            break;
        }

        // timeout
        if (!l_timeout) 
            break;

        ctime = millis();
        delta = ctime - stime;

        //yield();

    } while ( (!read_success) && (delta < l_timeout) );
    
    return read_success;
}

/* Parser */
bool M5ComSigfox :: getData_CheckOk(unsigned int timeout)
{
    char response[128];
    int  resp_len;

    if (!getData(response, &resp_len, timeout))
    {
        if (checkSerialLog())
            Serial.println("Response: Read data failed");

        return false;
    }
    if (!strstr(response, "OK"))
    {
        if (checkSerialLog())
        {
            char str[32];
            sprintf(str, "Response: NOT OK (data = %s, %d)", response, resp_len);
            Serial.println(str);
        }
        return false;
    }
    return true;
}

void M5ComSigfox :: logSerialEnable(bool enable)
{
    log_serial = enable;
}

bool M5ComSigfox :: checkSerialLog(void)
{
    return log_serial;
}

/* ***************** private functions ***************** */
int M5ComSigfox :: send_command(const char* command)
{
    int status = chk_command_seq();
    if (status) return status;

    if (log_serial) Serial.println(command);

    resetQueue();
    Serial2.flush();
    Serial2.print(command);
	Serial2.print("\r");
	
	return NO_ERROR;
}

int M5ComSigfox :: chk_command_seq(void)
{
    // check serial port
    if (!Serial2)
        return ERR_SERIAL_CLOSED;

    // try to wake MKR up
    if (fSleep)
    {
        fSleep = false;
        wakeup();
    }

    // clear RX buffer
    while (Serial2.available()) Serial2.read();

    return NO_ERROR;
}

/* uart 2 queue */
void  M5ComSigfox :: uart_write_queue(void)
{
    while (Serial2.available()) 
    {
        bool fNewLine = false;
        char c;

        if (checkBufferFull())
            break;

        c = Serial2.read();
        
        if (checkNewLineCharacter(c))
            fNewLine = true;
        
        if (( (c >= 0x20) && (c <= 0x7F) ) || fNewLine)
        {
            /* enqueue */
            write(c);

            /* write to TX (uart over usb) */
            if (checkSerialLog())
                Serial.write(c);

            if (fNewLine)
            {
                setOneLineReceived();

                /* set both of flag if we recv '\r' */
                if (c == '\r')
                    setResponseReceived();
                
                /* write to TX (uart over usb) */
                if (checkSerialLog())
                    Serial.println("");

                /* exit while-loop because the main flow parser needs to proccess the response data */
                break;
            }
        }
    }
}