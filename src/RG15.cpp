#include "RG15.hpp"
#include "Arduino.h"

/**
 * @brief Arduino library for the RG15 rain gauge sensor, written in C++
 * @author Paul Reichmuth (@PaulReichmuth)
 * @version 0.0.1
 * @date 2024-04-24
 */

/**
 * @brief Construct a new instance of a RG15 sensor
 *
 * @param serial The serial port to use (usually Serial1 or Serial2)
 * @param baud The baud rate of the sensor (default is 9600)
 */
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

/**
 * @brief Construct a new instance of a RG15 sensor with default baud rate of 9600
 *
 * @param serial The serial port to use (usually Serial1 or Serial2)
 */

RG15::RG15(HardwareSerial &serial) : RG15(serial, 9600) {}

RG15::~RG15() = default;

/**
 * @brief Clear the data in buffer
 *
 */
void RG15::_clearDataIn()
{
    this->_dataIn = "";
    this->serial.flush();
}

void RG15::_waitForSensorReady()
{
    delay(2000); // sensor needs a little time between commands for some reason, we can't just spam it. 2 seconds seems to be a good value (might change in the future)
}
/**
 * @brief Set the unit of the sensor
 *
 * @param isMetric true for metric, false for imperial
 * @return true if the unit was set and acknowledged by the sensor
 * @return false if the sensor did not acknowledge the unit change
 */
bool RG15::_setUnit(bool isMetric)
{
    if (isMetric)
    {
        this->serial.println("M");
        while (this->serial.available())
        {
            this->_dataIn += std::string(serial.readString().c_str());
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
        while (this->serial.available())
        {
            this->_dataIn += std::string(serial.readString().c_str());
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

/**
 * @brief Set the sensor to polling mode
 *
 * @return true if the polling mode was set and acknowledged by the sensor
 * @return false if the sensor did not acknowledge the polling mode
 */
bool RG15::_setPolling()
{
    this->serial.println("P");
    while (this->serial.available())
    {
        this->_dataIn += std::string(serial.readString().c_str());
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

/**
 * @brief Initialize the sensor
 *
 * @param polling true to enable polling, false otherwise
 * @param isMetric true for metric, false for imperial
 * @return true if the sensor was initialized successfully
 * @return false if not all commands were acknowledged by the sensor
 */
bool RG15::begin(bool polling, bool isMetric)
{
    this->serial.begin(this->_baud);
    this->_clearDataIn();
    if (!this->reboot())
    {
        this->_initErr = 1;
        return false;
    }
    this->_clearDataIn();
    if (!this->_setUnit(isMetric))
    {
        this->_initErr = 2;
        return false;
    }
    this->_clearDataIn();
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
    this->_initErr = 0;
    return true;
}

/**
 * @brief Initialize the sensor in polling mode and metric units
 *
 * @return true if the sensor was initialized successfully
 * @return false if not all commands were acknowledged by the sensor
 */
bool RG15::begin()
{
    this->serial.begin(this->_baud);
    this->_clearDataIn();
    if (!this->reboot())
    {
        this->_initErr = 1;
        return false;
    }
    this->_clearDataIn();
    this->_waitForSensorReady();
    if (!this->_setUnit(true))
    {
        this->_initErr = 2;
        return false;
    }
    this->_clearDataIn();
    this->_waitForSensorReady();

    if (!this->_setPolling())
    {
        this->_initErr = 3;
        return false;
    }
    else
    {
        this->_polling = true;
    }
    return true;
}

/**
 * @brief Get all rain gauge data in polling mode and store it in the sensor object
 *
 * @return true if the data was successfully retrieved
 * @return false if no data was recieved or polling mode was not set
 */
bool RG15::doPoll()
{
    this->_clearDataIn();
    if (this->_polling)
    {
        this->serial.println("R");
        while (this->serial.available())
        {
            this->_dataIn += std::string(serial.readString().c_str());
        }
        this->_acc = std::stof(this->_dataIn.substr(this->_dataIn.find_first_of("Acc") + 5, 4));
        this->_eventAcc = std::stof(this->_dataIn.substr(this->_dataIn.find_first_of("EventAcc") + 10, 4));
        this->_totalAcc = std::stof(this->_dataIn.substr(this->_dataIn.find_first_of("TotalAcc") + 10, 4));
        this->_rInt = std::stof(this->_dataIn.substr(this->_dataIn.find_first_of("RInt") + 6, 4));
        return true;
    }
    return false;
}

/**
 * @brief Get all rain gauge data in polling mode and store it in the given references, does not update getAcc(), getEventAcc(), getTotalAcc(), getRInt()!
 *
 * @param acc Pointer to a variable containing the accumulated rainfall since the last reset
 * @param eventAcc Pointer to a variable containing the accumulated rainfall since the last event
 * @param totalAcc Pointer to a variable containing the total accumulated rainfall
 * @param rInt Pointer to a variable containing the rainfall intensity
 *
 * @return true if the data was successfully retrieved
 * @return false if no data was recieved or polling mode was not set
 */
bool RG15::doPoll(float *acc, float *eventAcc, float *totalAcc, float *rInt)
{
    this->_clearDataIn();
    if (this->_polling)
    {
        this->serial.println("R");
        unsigned long start = millis();
        while (this->serial.available())
        {
            this->_dataIn += std::string(serial.readString().c_str());

            if (millis() - start > 1000)
            {
                return false;
            }
        }
        *acc = std::stof(this->_dataIn.substr(this->_dataIn.find_first_of("Acc") + 5, 4));
        *eventAcc = std::stof(this->_dataIn.substr(this->_dataIn.find_first_of("EventAcc") + 10, 4));
        *totalAcc = std::stof(this->_dataIn.substr(this->_dataIn.find_first_of("TotalAcc") + 10, 4));
        *rInt = std::stof(this->_dataIn.substr(this->_dataIn.find_first_of("RInt") + 6, 4));

        return true;
    }
    return false;
}

void RG15::resetTotalAcc()
{
    this->serial.println("O");
    while (this->serial.available())
    {
        this->_dataIn += std::string(serial.readString().c_str());
    }
    this->_totalAcc = 0;
    this->_clearDataIn();
}

/**
 * @brief Reboot the sensor via the "K" command and check if the sensor rebooted successfully
 *
 * @return true if sensor rebooted successfully
 * @return false if sensor did not respond to the reboot command
 */
bool RG15::reboot()
{
    this->serial.println("K");
    while (this->serial.available())
    {
        this->_dataIn += std::string(serial.readString().c_str());
    }
    if (this->_dataIn.find(std::string("RG-15")) == std::string::npos)
    {
        return false;
    }
    else
    {
        return true;
    }
}
String RG15::getDataIn()
{
    return this->_dataIn.c_str();
}

int RG15::getInitErr()
{
    return this->_initErr;
}
