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

#include "ComBufferQueue.h"

/* flag set, clear & check */
bool ComBufferQueue :: checkBufferFull(void)
{
    return buff_full;
}
void ComBufferQueue :: setResponseReceived  (void) 
{
    buff_response_completed = true; 
}
void ComBufferQueue :: clearResponseReceived(void) 
{
    buff_response_completed = false;
}
bool ComBufferQueue :: checkResponseReceived(void) 
{
    return buff_response_completed; 
}

void ComBufferQueue :: setOneLineReceived  (void)  
{
    buff_one_line_in_buff = true; 
}
void ComBufferQueue :: clearOneLineReceived(void)  
{
    buff_one_line_in_buff = false;
}
bool ComBufferQueue :: checkOneLineReceived(void)  
{
    return buff_one_line_in_buff; 
}

/*  */
void ComBufferQueue :: addNewLineCharacter(char c)
{
    for (int i = 0; i < 4; ++i)
    {
        if (!c || buff_change_line_character[i] == c)
        {
            // exist already or invalid character
            break;
        }

        if (buff_change_line_character[i] == 0)
        {
            //Serial.print("Add success\r\n");
            buff_change_line_character[i] = c;
            break;
        }
    }
}
void ComBufferQueue :: removeNewLineCharacter(char c)
{
    // invalid character
    if (!c) return;

    for (int i = 0; i < 4; ++i)
    {
        if (buff_change_line_character[i] == c)
        {
            buff_change_line_character[i] = 0;
            break;
        }
    }
}
bool ComBufferQueue :: checkNewLineCharacter(char c)
{
    bool rsp = false;
    
    for (int i = 0; i < 4; ++i)
    {
        if ( (c == '\r') || (buff_change_line_character[i] && (c == buff_change_line_character[i]) ) )
        {
            rsp = true;
            break;
        }
    }

    return rsp;
};
char ComBufferQueue :: getNewLineCharacter (int index)
{
    if (index >= 0 && index < 4)
        return buff_change_line_character[index];
    else
        return 0;
}


/* read one line from queue (until '\r')*/
bool ComBufferQueue :: getResponseData(char *pStream, int *pLen)
{
    return readOneLine(pStream, pLen, '\r');
}

/* read one line from queue */
bool ComBufferQueue :: readOneLine(char *pStream, int *pLen, char chr)
{
    char c;
    if (!pStream) return false;
    if (pLen) *pLen = 0;
    
    while (1)
    {
        c = read();
        
        // check end of line
        if (c != chr)
        {
            if (c == '\r' || c == chr)
                break;
        }
        else if (c == '\r')
        {
            break;
        }
        else if (c == '\n')
        {
            continue;
        }
        
        // check EOF
        if (c == 0xFF)
            break;

        *(pStream++) = c;
        
        if (pLen)
        {
          (*pLen)++;
        }
    }

    return (*pLen > 0) ? true : false;
}

/* write N-bytes data to queue */
bool ComBufferQueue :: writeQueue(char *c, int len)
{
    while (len && !buff_full)
    {
        buff_stack[buff_inx++] = *c;
        len--;
        c++;
        buff_length++;

        if (buff_inx >= buff_max)
            buff_inx = 0;

        if (buff_inx == buff_outx)
            buff_full = true;
    }

    return (len > 0) ? false : true;
}

/* read data from queue */
int ComBufferQueue :: readQueue(char *c, int len)
{
    int read_num = 0;
    
    while (len > 0 && buff_length > 0)
    {
        *c = buff_stack[buff_outx++];
        buff_length--;
        len--;
        c++;
        read_num++;

        if (buff_outx >= buff_max)
            buff_outx = 0;

        buff_full = false;
    }

    return read_num;
}

/* write one byte to queue */
bool ComBufferQueue :: write(char c) 
{
    if (!buff_full)
    {
        buff_stack[buff_inx++] = c;
        buff_length++;
        if (buff_inx >= buff_max)
            buff_inx = 0;

        if (buff_inx == buff_outx)
            buff_full = true;
    }
    return buff_full;
}
/* read one byte */
char ComBufferQueue :: read() 
{
    char c = 0xFF;

    if (buff_length > 0)
    {
        c = buff_stack[buff_outx++];
        buff_length--;
        
        if (buff_outx >= buff_max)
            buff_outx = 0;

        buff_full = false;
    }

    return c;
}
