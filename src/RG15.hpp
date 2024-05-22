#include "Arduino.h"
#include <string>


/**
 * @brief Arduino library for the RG15 rain gauge sensor, written in C++
 * @author Paul Reichmuth (@PaulReichmuth)
 * @version 0.0.2
 * @date 2024-05-22
 */
class RG15
{
private:
    const int _baud; //baud rate of the sensor
    std::string _dataIn; //buffer for the sensor response
    HardwareSerial &serial; //serial port to use
    bool _polling; //whether the sensor is in polling mode
    bool _isMetric; //whether the sensor is in metric mode

    float _acc;      // accumulated rainfall since last poll
    float _eventAcc; // accumulated rainfall in the current event
    float _totalAcc; // total accumulated rainfall since last reset
    float _rInt;     // rainfall intensity

    /**
     * @brief Set high resolution 
     * 
     */
    bool _setHighResolution();

    /**
     * @brief Set low resolution
     * 
     */
    bool _setLowResolution();

    /**
     * @brief Set the unit of the sensor
     *
     * @param isMetric true for metric, false for imperial
     * @return true if the unit was set and acknowledged by the sensor
     * @return false if the sensor did not acknowledge the unit change
     */
    bool _setUnit(bool isMetric);

    /**
     * @brief Set the sensor to polling mode
     *
     * @return true if the polling mode was set and acknowledged by the sensor
     * @return false if the sensor did not acknowledge the polling mode
     */
    bool _setPolling();

    /**
     * @brief check if the sensor is ready to receive commands and has reset correctly
     * 
     * @return true if the sensor is ready
     * @return false if we didnt get the expected response from the sensor and a reboot attempt failed
     */
    bool _checkSensorReady();

    /**
     * @brief Read the response from the sensor and store it in the dataIn variable
     *
     * @return true if the response was read successfully
     * @return false if the sensor did not respond after 10 seconds
     */
    bool _readSensorResponse();
    
    /**
     * @brief clean up the dataIn buffer
     * 
     */
    void _clearDataIn();

    int _initErr; //Variable to store the initialization error (0 = no error, 1 = reboot error, 2 = unit error, 3 = polling error, 4 = resolution error)

public:
    /**
     * @brief Construct a new instance of a RG15 sensor
     *
     * @param serial The serial port to use (usually Serial1 or Serial2)
     * @param baud The baud rate of the sensor (default is 9600)
     */
    RG15(HardwareSerial &serial, int baud);

    /**
     * @brief Construct a new instance of a RG15 sensor with default baud rate of 9600
     *
     * @param serial The serial port to use (usually Serial1 or Serial2)
     */
    RG15(HardwareSerial &serial);

    /**
     * @brief Initialize the sensor
     *
     * @param polling true to enable polling, false otherwise
     * @param isMetric true for metric, false for imperial
     * @param isHighRes true for high resolution, false for low resolution
     * @return true if the sensor was initialized successfully
     * @return false if not all commands were acknowledged by the sensor
     */
    bool begin(bool polling, bool isMetric, bool isHighRes);

    /**
     * @brief Initialize the sensor in polling mode, high resolution and metric units
     *
     * @return true if the sensor was initialized successfully
     * @return false if not all commands were acknowledged by the sensor
     */
    bool begin();

    /**
     * @brief Get all rain gauge data in polling mode and store it in the sensor object
     *
     * @return true if the data was successfully retrieved
     * @return false if no data was recieved or polling mode was not set
     */
    bool doPoll();

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
    bool doPoll(float *acc, float *eventAcc, float *totalAcc, float *rInt);

    /**
     * @brief Reboot the sensor via the "K" command and check if the sensor rebooted successfully
     *
     * @return true if sensor rebooted successfully
     * @return false if sensor did not respond to the reboot command
     */
    bool reboot();

    /**
     * @brief Reset the total accumulated rainfall inside the sensor and update the totalAcc variable
     *
     */
    void resetTotalAcc();

    /**
     * @brief Get the dataIn buffer (for debugging purposes)
     *
     * @return std::string The dataIn buffer
     */
    String getDataIn();

    /**
     * @brief Get the Init Error
     *
     * @return Init Error code as int (0 = no error, 1 = reboot error, 2 = unit error, 3 = polling error, 4 = resolution error)
     */
    int getInitErr();

    /**
     * @brief Get the accumulated rainfall since the last poll
     *
     * @return float The accumulated rainfall since the last poll
     */
    float getAcc();

    /**
     * @brief Get the accumulated rainfall in the current event
     *
     * @return float The accumulated rainfall in the current event
     */
    float getEventAcc();

    /**
     * @brief Get the total accumulated rainfall since the last reset
     *
     * @return float The total accumulated rainfall since the last reset
     */
    float getTotalAcc();

    /**
     * @brief Get the rainfall intensity
     *
     * @return float The rainfall intensity
     */
    float getRInt();
};
