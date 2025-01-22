#include <string>

#include "Arduino.h"

/**
 * @brief Arduino library for the RG15 rain gauge sensor, written in C++
 * @author Paul Reichmuth (@PaulReichmuth) with additional changes from
 * Jan-Patrick Bollow(@JBollow) and Björn Luig (@BjoernLuig)
 * @version 0.0.2
 * @date 2024-05-22
 * @note See official documentation of the RG15:
 * https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2
 */

class RG15 {
 private:
  HardwareSerial &serial;  // serial port to use
  float _acc;              // accumulated rainfall since last poll
  float _eventAcc;         // accumulated rainfall in the current event
  float _totalAcc;         // total accumulated rainfall since last reset
  float _rInt;             // rainfall intensity

  /**
   * @brief Send a command letter to the sensor
   *
   * @param a the command character in lower case
   * @return true if the sensor acknowledges the command letter
   * @return false if the sensor did not acknowledge the command letter
   */
  bool _sendCommandLetter(char a);

  //   /**
  //    * @brief check if the sensor is ready to receive commands and has reset
  //    * correctly
  //    *
  //    * @return true if the sensor is ready
  //    * @return false if we didnt get the expected response from the sensor
  //    and a
  //    * reboot attempt failed
  //    */
  //   bool _checkSensorReady();

  //   /**
  //    * @brief Read the response from the sensor and store it in the dataIn
  //    * variable
  //    *
  //    * @return true if the response was read successfully
  //    * @return false if the sensor did not respond after 10 seconds
  //    */
  //   bool _readSensorResponse();

  //   /**
  //    * @brief clean up the dataIn buffer
  //    *
  //    */
  //   void _clearDataIn();

  //   int _initErr;  // Variable to store the initialization error (0 = no
  //   error, 1
  //                  // = reboot error, 2 = unit error, 3 = polling error, 4 =
  //                  // resolution error)

 public:
  /**
   * @brief Construct a new instance of a RG15 sensor
   *
   * @param serial The serial port to use (usually Serial1 or Serial2)
   * @param baud The baud rate of the sensor (default is 9600)
   */
  RG15(HardwareSerial &serial, int baud);

  /**
   * @brief Construct a new instance of a RG15 sensor with default baud rate of
   * 9600
   *
   * @param serial The serial port to use (usually Serial1 or Serial2)
   */
  RG15(HardwareSerial &serial);

  /**
   * @brief Initialize the sensor
   *
   * @param isHighPolling true for polling mode, false for cintinous mode
   * @param isMetric true for metric, false for imperial
   * @param isHighRes true for high resolution, false for low resolution
   * @return true if the sensor was initialized successfully
   * @return false if not all commands were acknowledged by the sensor
   */
  bool begin(bool isPolling, bool isMetric, bool isHighRes);

  /**
   * @brief Initialize the sensor in polling mode, high resolution and metric
   * units
   *
   * @return true if the sensor was initialized successfully
   * @return false if not all commands were acknowledged by the sensor
   */
  bool begin(); /**
                 * @brief Set high resolution
                 *
                 * @return true if the reselution was set and acknowledged by
                 * the sensor
                 * @return false if the sensor did not acknowledge the
                 * reselution change
                 */
  bool setHighResolution();

  /**
   * @brief Set low resolution
   *
   * @return true if the reselution was set and acknowledged by the sensor
   * @return false if the sensor did not acknowledge the reselution change
   */
  bool setLowResolution();

  /**
   * @brief Set the unit of the sensor to metric
   *
   * @return true if the unit was set and acknowledged by the sensor
   * @return false if the sensor did not acknowledge the unit change
   */
  bool setUnitMetric(bool isMetric);

  /**
   * @brief Set the unit of the sensorto imperial
   *
   * @return true if the unit was set and acknowledged by the sensor
   * @return false if the sensor did not acknowledge the unit change
   */
  bool setUnitImperial(bool isMetric);

  /**
   * @brief Set the sensor to polling mode
   *
   * @return true if the polling mode was set and acknowledged by the sensor
   * @return false if the sensor did not acknowledge the polling mode
   */
  bool setPolling();

  /**
   * @brief Set the sensor to continuous mode
   *
   * @return true if the continuous mode was set and acknowledged by the sensor
   * @return false if the sensor did not acknowledge the continuous mode
   */
  bool setContinuous();

  /**
   * @brief Get all rain gauge data in polling mode and store it in the sensor
   * object
   *
   * @return true if the data was successfully retrieved
   * @return false if no data was recieved or polling mode was not set
   */
  bool doPoll();

  //   /**
  //    * @brief Get all rain gauge data in polling mode and store it in the
  //    given
  //    * references, does not update getAcc(), getEventAcc(), getTotalAcc(),
  //    * getRInt()!
  //    *
  //    * @param acc Pointer to a variable containing the accumulated rainfall
  //    since
  //    * the last reset
  //    * @param eventAcc Pointer to a variable containing the accumulated
  //    rainfall
  //    * since the last event
  //    * @param totalAcc Pointer to a variable containing the total accumulated
  //    * rainfall
  //    * @param rInt Pointer to a variable containing the rainfall intensity
  //    *
  //    * @return true if the data was successfully retrieved
  //    * @return false if no data was recieved or polling mode was not set
  //    */
  //   bool doPoll(float *acc, float *eventAcc, float *totalAcc, float *rInt);

  /**
   * @brief Reboot the sensor via the "K" command and check if the sensor
   * rebooted successfully
   *
   * @return true if sensor rebooted successfully
   * @return false if sensor did not respond to the reboot command
   */
  bool reboot();

  /**
   * @brief Reset the total accumulated rainfall inside the sensor and update
   * the totalAcc variable
   *
   * @return true if the reset was succesfull
   * @return false if the reset was not succesfull
   */
  void resetTotalAcc();

  //   /**
  //    * @brief Get the dataIn buffer (for debugging purposes)
  //    *
  //    * @return std::string The dataIn buffer
  //    */
  //   String getDataIn();

  //   /**
  //    * @brief Get the Init Error
  //    *
  //    * @return Init Error code as int (0 = no error, 1 = reboot error, 2 =
  //    unit
  //    * error, 3 = polling error, 4 = resolution error)
  //    */
  //   int getInitErr();

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
   * @brief Get the currunt rainfall intensity
   *
   * @return float The currunt rainfall intensity
   */
  float getRInt();
};
