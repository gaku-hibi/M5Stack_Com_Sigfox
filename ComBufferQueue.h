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
#ifndef __COM_BUFFER_QUEUE__
#define __COM_BUFFER_QUEUE__

/* queue */
class ComBufferQueue
{
public:
    /* constructure */
    ComBufferQueue()
    {
        resetQueue();
    	for (int i = 0; i < 4; i++) {
    		buff_change_line_character[i] = 0;
    	}
    };
    
    /* destructure */
    ~ComBufferQueue(){};

    /* reset queue */
    void resetQueue(void)
    {
        buff_length = 
        buff_inx    = 
        buff_outx   = 0;
        buff_full   = false;
        buff_one_line_in_buff = false;
        buff_response_completed = false;
    };

protected:
    /* flag setting, clearing and checking */
    bool checkBufferFull(void);
    void setResponseReceived  (void);
    void clearResponseReceived(void);
    bool checkResponseReceived(void);

    void setOneLineReceived  (void);
    void clearOneLineReceived(void);
    bool checkOneLineReceived(void);

    void addNewLineCharacter(char c);
    void removeNewLineCharacter(char c);
    bool checkNewLineCharacter(char c);
    char getNewLineCharacter(int index);
    
    bool getResponseData(char *pStream, int *pLen);
    bool readOneLine(char *pStream, int *pLen, char chr);

    bool writeQueue(char *c, int len);
    int  readQueue(char *c, int len);
    bool write(char c) ;
    char read() ;

private:
    // queue stack & control
    const int buff_max = 128*4;
    char buff_stack[128*4];
    bool buff_full;
    int  buff_length;
    int  buff_inx;
    int  buff_outx;
    
    // flags
    bool buff_one_line_in_buff;
    bool buff_response_completed;

    // new-line character 
    char buff_change_line_character[4];
};

#endif //__COM_BUFFER_QUEUE__
