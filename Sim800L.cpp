#include "Arduino.h"
#include "Sim800L.h"
#include <SoftwareSerial.h>

Sim800L::Sim800L(void) : SoftwareSerial(DEFAULT_RX_PIN, DEFAULT_TX_PIN)
{
    RX_PIN 		= DEFAULT_RX_PIN;
    TX_PIN 		= DEFAULT_TX_PIN;
    RESET_PIN 	= DEFAULT_RESET_PIN;
    LED_PIN 	= DEFAULT_LED_PIN;
    LED_FLAG 	= DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx) : SoftwareSerial(rx, tx)
{
    RX_PIN 		= rx;
    TX_PIN 		= tx;
    RESET_PIN 	= DEFAULT_RESET_PIN;
    LED_PIN 	= DEFAULT_LED_PIN;
    LED_FLAG 	= DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx, uint8_t rst) : SoftwareSerial(rx, tx)
{
    RX_PIN 		= rx;
    TX_PIN 		= tx;
    RESET_PIN 	= rst;
    LED_PIN 	= DEFAULT_LED_PIN;
    LED_FLAG 	= DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx, uint8_t rst, uint8_t led) : SoftwareSerial(rx, tx)
{
    RX_PIN 		= rx;
    TX_PIN 		= tx;
    RESET_PIN 	= rst;
    LED_PIN 	= led;
    LED_FLAG 	= true;
}

void Sim800L::begin()
{

    pinMode(RESET_PIN, OUTPUT);

    _baud = DEFAULT_BAUD_RATE;			
    this->SoftwareSerial::begin(_baud);

    _sleepMode = 0;
    _functionalityMode = 1;

    if (LED_FLAG) pinMode(LED_PIN, OUTPUT);

    _buffer.reserve(BUFFER_RESERVE_MEMORY);
}

void Sim800L::begin(uint32_t baud)
{

    pinMode(RESET_PIN, OUTPUT);

    _baud = baud;
    this->SoftwareSerial::begin(_baud);

    _sleepMode = 0;
    _functionalityMode = 1;

    if (LED_FLAG) pinMode(LED_PIN, OUTPUT);

    _buffer.reserve(BUFFER_RESERVE_MEMORY); 
}


bool Sim800L::setSleepMode(bool state)
{

    _sleepMode = state;

    if (_sleepMode) this->SoftwareSerial::print(F("AT+CSCLK=1\r\n "));
    else 			this->SoftwareSerial::print(F("AT+CSCLK=0\r\n "));

    if ( (_readSerial().indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    
}

bool Sim800L::getSleepMode()
{
    return _sleepMode;
}

bool Sim800L::setFunctionalityMode(uint8_t fun)
{

    if (fun==0 || fun==1 || fun==4)
    {

        _functionalityMode = fun;

        switch(_functionalityMode)
        {
        case 0:
            this->SoftwareSerial::print(F("AT+CFUN=0\r\n "));
            break;
        case 1:
            this->SoftwareSerial::print(F("AT+CFUN=1\r\n "));
            break;
        case 4:
            this->SoftwareSerial::print(F("AT+CFUN=4\r\n "));
            break;
        }

        if ( (_readSerial().indexOf("ER")) == -1)
        {
            return false;
        }
        else return true;
        
    }
    return false;
}

uint8_t Sim800L::getFunctionalityMode()
{
    return _functionalityMode;
}

bool Sim800L::setPIN(String pin)
{
    String command;
    command  = "AT+CPIN=";
    command += pin;
    command += "\r";

    
    this->SoftwareSerial::print(command);

    if ( (_readSerial(5000).indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    
}


String Sim800L::getProductInfo()
{
    this->SoftwareSerial::print("ATI\r");
    return (_readSerial());
}


String Sim800L::getOperatorsList()
{

    
    this->SoftwareSerial::print("AT+COPS=?\r");

    return _readSerial(45000);

}

String Sim800L::getOperator()
{

    this->SoftwareSerial::print("AT+COPS ?\r");

    return _readSerial();

}


bool Sim800L::calculateLocation()
{
 

    uint8_t type = 1;
    uint8_t cid = 1;
	
	String tmp = "AT+CIPGSMLOC=" + String(type) + "," + String(cid) + "\r\n";
	this->SoftwareSerial::print(tmp);
	
	

    String data = _readSerial(20000);

    if (data.indexOf("ER")!=(-1)) return false;

    uint8_t indexOne;
    uint8_t indexTwo;

    indexOne = data.indexOf(":") + 1;
    indexTwo = data.indexOf(",");

    _locationCode = data.substring(indexOne, indexTwo);

    indexOne = data.indexOf(",") + 1;
    indexTwo = data.indexOf(",", indexOne);

    _longitude = data.substring(indexOne, indexTwo);

    indexOne = data.indexOf(",", indexTwo) + 1;
    indexTwo = data.indexOf(",", indexOne);

    _latitude = data.substring(indexOne, indexTwo);

    return true;

}

String Sim800L::getLocationCode()
{
    return _locationCode;
  
}

String Sim800L::getLongitude()
{
    return _longitude;
}

String Sim800L::getLatitude()
{
    return _latitude;
}



void Sim800L::reset()
{
    if (LED_FLAG) digitalWrite(LED_PIN,1);

    digitalWrite(RESET_PIN,1);
    delay(1000);
    digitalWrite(RESET_PIN,0);
    delay(1000);
    
    this->SoftwareSerial::print(F("AT\r\n"));
    while (_readSerial().indexOf("OK")==-1 )
    {
        this->SoftwareSerial::print(F("AT\r\n"));
    }

    while (_readSerial().indexOf("SMS")==-1 );

    if (LED_FLAG) digitalWrite(LED_PIN,0);

}

void Sim800L::setPhoneFunctionality()
{
    this->SoftwareSerial::print (F("AT+CFUN=1\r\n"));
}


String Sim800L::signalQuality()
{
    this->SoftwareSerial::print (F("AT+CSQ\r\n"));
    return(_readSerial());
}


void Sim800L::activateBearerProfile()
{
    this->SoftwareSerial::print (F(" AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" \r\n" ));
    _buffer=_readSerial();  
    this->SoftwareSerial::print (F(" AT+SAPBR=3,1,\"APN\",\"internet\" \r\n" ));
    _buffer=_readSerial();  
    this->SoftwareSerial::print (F(" AT+SAPBR=1,1 \r\n"));
    delay(1200);
    _buffer=_readSerial();			
    this->SoftwareSerial::print (F(" AT+SAPBR=2,1\r\n "));
    delay(3000);
    _buffer=_readSerial(); 			
}


void Sim800L::deactivateBearerProfile()
{
    this->SoftwareSerial::print (F("AT+SAPBR=0,1\r\n "));
    delay(1500);
}



bool Sim800L::answerCall()
{
    this->SoftwareSerial::print (F("ATA\r\n"));
    
    if ( (_readSerial().indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    
}


void  Sim800L::callNumber(char* number)
{
    this->SoftwareSerial::print (F("ATD"));
    this->SoftwareSerial::print (number);
    this->SoftwareSerial::print (F(";\r\n"));
}



uint8_t Sim800L::getCallStatus()
{
    
    this->SoftwareSerial::print (F("AT+CPAS\r\n"));
    _buffer=_readSerial();
    return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();

}



bool Sim800L::hangoffCall()
{
    this->SoftwareSerial::print (F("ATH\r\n"));
    _buffer=_readSerial();
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    
}


bool Sim800L::sendSms(char* number,char* text)
{

    this->SoftwareSerial::print (F("AT+CMGF=1\r")); 	
    _buffer=_readSerial();
    this->SoftwareSerial::print (F("AT+CMGS=\""));  	
    this->SoftwareSerial::print (number);
    this->SoftwareSerial::print(F("\"\r"));
    _buffer=_readSerial();
    this->SoftwareSerial::print (text);
    this->SoftwareSerial::print ("\r");
    _buffer=_readSerial();
    this->SoftwareSerial::print((char)26);
    _buffer=_readSerial(60000);
    if ((_buffer.indexOf("ER")) != -1) {
        return true;
    } else if ((_buffer.indexOf("CMGS")) != -1) {
        return false;
  	} else {
    	return true;
  	}
    
}


bool Sim800L::prepareForSmsReceive()
{
	this->SoftwareSerial::print(F("AT+CMGF=1\r"));
    _buffer=_readSerial();
    if((_buffer.indexOf("OK")) == -1)
    {
        return false;
    }
	this->SoftwareSerial::print(F("AT+CNMI=2,1,0,0,0\r"));
    _buffer=_readSerial();
    if((_buffer.indexOf("OK")) == -1)
    {
        return false;
    }
    return true;
}

const uint8_t Sim800L::checkForSMS()
{
	 _buffer = _readSerial(100);
	 if(_buffer.length() == 0)
	 {
	 	return 0;
	 }
     _buffer += _readSerial(1000);
	 
	 if(_buffer.indexOf("CMTI") == -1)
	 {
	 	return 0;
	 }
	 return _buffer.substring(_buffer.indexOf(',')+1).toInt();
}


String Sim800L::getNumberSms(uint8_t index)
{
    _buffer=readSms(index);
    
    if (_buffer.length() > 10) 
    {
        uint8_t _idx1=_buffer.indexOf("+CMGR:");
        _idx1=_buffer.indexOf("\",\"",_idx1+1);
        return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+4));
    }
    else
    {
        return "";
    }
}



String Sim800L::readSms(uint8_t index)
{

    if(( _readSerial(5000).indexOf("ER")) != -1)
    {
    	return "";
    }

    this->SoftwareSerial::print (F("AT+CMGR="));
    this->SoftwareSerial::print (index);
    this->SoftwareSerial::print ("\r");
    _buffer=_readSerial();
    if (_buffer.indexOf("CMGR") == -1)
    {
    	return "";
    }

	_buffer = _readSerial(10000);
	byte first = _buffer.indexOf('\n', 2) + 1;
	byte second = _buffer.indexOf('\n', first);
    return _buffer.substring(first, second);
}


bool Sim800L::delAllSms()
{
    

    this->SoftwareSerial::print(F("at+cmgda=\"del all\"\n\r"));
    _buffer=_readSerial(25000);
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    
}


void Sim800L::RTCtime(int *day,int *month, int *year,int *hour,int *minute, int *second)
{
    this->SoftwareSerial::print(F("at+cclk?\r\n"));
    
    _buffer=_readSerial();
    if ((_buffer.indexOf("ERR"))!=-1)
    {
        delay(50);
        this->SoftwareSerial::print(F("at+cclk?\r\n"));
    }
    if ((_buffer.indexOf("ERR"))==-1)
    {
        _buffer=_buffer.substring(_buffer.indexOf("\"")+1,_buffer.lastIndexOf("\"")-1);
        *year=_buffer.substring(0,2).toInt();
        *month= _buffer.substring(3,5).toInt();
        *day=_buffer.substring(6,8).toInt();
        *hour=_buffer.substring(9,11).toInt();
        *minute=_buffer.substring(12,14).toInt();
        *second=_buffer.substring(15,17).toInt();
    }
}


String Sim800L::dateNet()
{
    this->SoftwareSerial::print(F("AT+CIPGSMLOC=2,1\r\n "));
    _buffer=_readSerial();

    if (_buffer.indexOf("OK")!=-1 )
    {
        return _buffer.substring(_buffer.indexOf(":")+2,(_buffer.indexOf("OK")-4));
    }
    else
        return "0";
}


bool Sim800L::updateRtc(int utc)
{

    activateBearerProfile();
    _buffer=dateNet();
    deactivateBearerProfile();

    _buffer=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length());
    String dt=_buffer.substring(0,_buffer.indexOf(","));
    String tm=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length()) ;

    int hour = tm.substring(0,2).toInt();
    int day = dt.substring(8,10).toInt();

    hour=hour+utc;

    String tmp_hour;
    String tmp_day;
    
    if (hour<0)
    {
        hour+=24;
        day-=1;
    }
    if (hour<10)
    {

        tmp_hour="0"+String(hour);
    }
    else
    {
        tmp_hour=String(hour);
    }
    if (day<10)
    {
        tmp_day="0"+String(day);
    }
    else
    {
        tmp_day=String(day);
    }
    
    this->SoftwareSerial::print("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
    if ( (_readSerial().indexOf("ER"))!=-1)
    {
        return true;
    }
    else return false;


}




String Sim800L::_readSerial()
{

    uint64_t timeOld = millis();

    while (!this->SoftwareSerial::available() && !(millis() > timeOld + TIME_OUT_READ_SERIAL))
    {
        delay(13);
    }

    String str;

    while(this->SoftwareSerial::available())
    {
        if (this->SoftwareSerial::available()>0)
        {
            str += (char) this->SoftwareSerial::read();
        }
    }

    return str;

}

String Sim800L::_readSerial(uint32_t timeout)
{

    uint64_t timeOld = millis();

    while (!this->SoftwareSerial::available() && !(millis() > timeOld + timeout))
    {
        delay(13);
    }

    String str;

    while(this->SoftwareSerial::available())
    {
        if (this->SoftwareSerial::available()>0)
        {
            str += (char) this->SoftwareSerial::read();
        }
    }

    return str;

}

