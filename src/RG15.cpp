#include "RG15.hpp"
#include "Arduino.h"

/**
 * @brief Arduino library for the RG15 rain gauge sensor, written in C++
 * @author Paul Reichmuth (@PaulReichmuth)
 * @version 0.0.2
 * @date 2024-05-22
 */


//________________________________________Begin constructor____________________________________________________
RG15::RG15(HardwareSerial &serial, int baud) : serial(serial), _baud(baud)
{
    this->_dataIn = "";
    this->_polling = false;
    this->_acc = 0;
    this->_eventAcc = 0;
    this->_totalAcc = 0;
    this->_rInt = 0;
    this->_initErr = 0;
}

RG15::RG15(HardwareSerial &serial) : RG15(serial, 9600) {}

//________________________________________Begin private member functions____________________________________________________

void RG15::_clearDataIn()
{
    this->_dataIn = "";
}

bool RG15::_readSensorResponse()
{
    this->_clearDataIn(); // make sure the buffer is empty so we don't confuse old data with new data
    unsigned long start = millis();
    while (this->_dataIn.empty())
    {
        while (this->serial.available())
        {
            this->_dataIn += std::string(this->serial.readString().c_str());
        }
        if (millis() - start > 10000)
        {
            return false; // if no response is received after 10 seconds, we assume the sensor is gone
        }
    }
    return true;
}

bool RG15::_setUnit(bool isMetric)
{
    if (isMetric)
    {
        this->serial.println("M");
        if (!this->_readSensorResponse())
        {
            return false;
        }
        if (this->_dataIn.find(std::string("m")) == std::string::npos)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        this->serial.println("I");
        if (!this->_readSensorResponse())
        {
            return false;
        }
        if (this->_dataIn.find(std::string("i")) == std::string::npos)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

bool RG15::_setPolling()
{
    this->serial.println("P");
    if (!this->_readSensorResponse())
    {
        return false;
    }
    if (this->_dataIn.find(std::string("p")) == std::string::npos)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool RG15::_checkSensorReady()
{
    if (!this->_readSensorResponse())
    {
        return false;
    }
    if (this->_dataIn.find(std::string("RG-15")) == std::string::npos)
    {
        if (this->_dataIn.find(std::string("Reset")) == std::string::npos)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return true;
}

bool RG15::_setHighResolution()
{
    this->serial.println("H");
    if (!this->_readSensorResponse())
    {
        return false;
    }
    if (this->_dataIn.find(std::string("h")) == std::string::npos)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool RG15::_setLowResolution()
{
    this->serial.println("L");
    if (!this->_readSensorResponse())
    {
        return false;
    }
    if (this->_dataIn.find(std::string("l")) == std::string::npos)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//________________________________________Begin public member functions____________________________________________________

bool RG15::begin(bool polling, bool isMetric, bool isHighRes)
{
    this->serial.begin(this->_baud);
    if (!this->_checkSensorReady())
    {
        if (!this->reboot())
        {
            this->_initErr = 1;
            return false;
        }
    }
    if (!this->_setUnit(isMetric))
    {
        this->_initErr = 2;
        return false;
    }
    if (polling)
    {
        if (!this->_setPolling())
        {
            this->_initErr = 3;
            return false;
        }
        else
        {
            this->_polling = true;
        }
    }
    if (isHighRes)
    {
        if (!this->_setHighResolution())
        {
            this->_initErr = 4;
            return false;
        }
    }
    else
    {
        if (!this->_setLowResolution())
        {
            this->_initErr = 4;
            return false;
        }
    }
    this->_initErr = 0;
    return true;
}

bool RG15::begin()
{
    this->serial.begin(this->_baud);
    this->_clearDataIn();
    if (!this->_checkSensorReady())
    {
        if (!this->reboot())
        {
            this->_initErr = 1;
            return false;
        }
    }
    if (!this->_setUnit(true))
    {
        this->_initErr = 2;
        return false;
    }
    if (!this->_setPolling())
    {
        this->_initErr = 3;
        return false;
    }
    else
    {
        this->_polling = true;
    }
    if (!this->_setHighResolution())
    {
        this->_initErr = 4;
        return false;
    }
    return true;
}

bool RG15::doPoll()
{
    if (this->_polling)
    {
        this->serial.println("R");
        if (!this->_readSensorResponse())
        {
            return false;
        }
        this->_acc = std::stof(this->_dataIn.substr(this->_dataIn.find(std::string("Acc")) + 5, 4));
        this->_eventAcc = std::stof(this->_dataIn.substr(this->_dataIn.find(std::string("EventAcc")) + 10, 4));
        this->_totalAcc = std::stof(this->_dataIn.substr(this->_dataIn.find(std::string("TotalAcc")) + 10, 4));
        this->_rInt = std::stof(this->_dataIn.substr(this->_dataIn.find(std::string("RInt")) + 6, 4));
        return true;
    }
    return false;
}

bool RG15::doPoll(float *acc, float *eventAcc, float *totalAcc, float *rInt)
{
    if(this->doPoll())
    {
        *acc = this->_acc;
        *eventAcc = this->_eventAcc;
        *totalAcc = this->_totalAcc;
        *rInt = this->_rInt;
        return true;
    }
    return false;
}

bool RG15::reboot()
{
    this->serial.println("K");
    if (!this->_readSensorResponse())
    {
        return false;
    }
    if (this->_dataIn.find(std::string("RG-15")) == std::string::npos)
    {
        if (this->_dataIn.find(std::string("Reset")) == std::string::npos)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return true;
    }
}

void RG15::resetTotalAcc(){
    this->serial.println("O");
    this->_readSensorResponse();
    this->doPoll();
}

String RG15::getDataIn()
{
    return this->_dataIn.c_str();
}

int RG15::getInitErr()
{
    return this->_initErr;
}

float RG15::getAcc()
{
    return this->_acc;
}

float RG15::getEventAcc()
{
    return this->_eventAcc;
}

float RG15::getTotalAcc()
{
    return this->_totalAcc;
}

float RG15::getRInt()
{
    return this->_rInt;
}
