#ifndef Sim800L_h
#define Sim800L_h
#include <SoftwareSerial.h>
#include "Arduino.h"


#define DEFAULT_RX_PIN 		2
#define DEFAULT_TX_PIN 		3
#define DEFAULT_RESET_PIN 	4		

#define DEFAULT_LED_FLAG	true 	
#define DEFAULT_LED_PIN 	13 		

#define BUFFER_RESERVE_MEMORY	255
#define DEFAULT_BAUD_RATE		9600
#define TIME_OUT_READ_SERIAL	5000

class Sim800L : public SoftwareSerial
{
private:

    uint32_t _baud;
    int _timeout;
    String _buffer;
    bool _sleepMode;
    uint8_t _functionalityMode;
    String _locationCode;
    String _longitude;
    String _latitude;

    String _readSerial();
    String _readSerial(uint32_t timeout);

public:

    uint8_t	RX_PIN;
    uint8_t TX_PIN;
    uint8_t RESET_PIN;
    uint8_t LED_PIN;
    bool	LED_FLAG;

    Sim800L(void);
    Sim800L(uint8_t rx, uint8_t tx);
    Sim800L(uint8_t rx, uint8_t tx, uint8_t rst);
    Sim800L(uint8_t rx, uint8_t tx, uint8_t rst, uint8_t led);

    void begin();					
    void begin(uint32_t baud);
    void reset();

    bool setSleepMode(bool state);
    bool getSleepMode();
    bool setFunctionalityMode(uint8_t fun);
    uint8_t getFunctionalityMode();

    bool setPIN(String pin);
    String getProductInfo();

    String getOperatorsList();
    String getOperator();

    bool calculateLocation();
    String getLocationCode();
    String getLongitude();
    String getLatitude();

    bool answerCall();
    void callNumber(char* number);
    bool hangoffCall();
    uint8_t getCallStatus();

	const uint8_t checkForSMS();
	bool prepareForSmsReceive();
    bool sendSms(char* number,char* text);
    String readSms(uint8_t index);
    String getNumberSms(uint8_t index);
    bool delAllSms();


    String signalQuality();
    void setPhoneFunctionality();
    void activateBearerProfile();
    void deactivateBearerProfile();
    bool setMode();

    void RTCtime(int *day,int *month, int *year,int *hour,int *minute, int *second);
    String dateNet();
    bool updateRtc(int utc);

};

#endif
