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
  HardwareSerial* _serial;  // pointer onto serial port of the sensor
  const unsigned int _baudRates[7] = {
      1200,  2400,  4800, 9600,
      19200, 38400, 57600};   // valid baud rates indexed by baude codes (0-6)
  char _responseBuffer[100];  // buffer to store the last sensor response

  unsigned long _timeout;  // timeout for sensor responses
  unsigned int _attempts;  // attempts at sensor communication
  char _unit;  // unit as 'm' for metric in mm, 'i' for imperial in inch or '?
               // if unknown
  float _acc;  // accumulated rainfall since last poll
  float _eventAcc;  // accumulated rainfall in the current event
  float _totalAcc;  // total accumulated rainfall since last reset
  float _rInt;      // rainfall intensity
  int _errorCode;   // Code of the latest error for debugging purposes

  bool _changeSettings(char setting);
  bool _prepSerial();
  bool _sendCommand(char command);
  bool _sendCommand(char* command);
  bool _collectResponse();
  bool _isAsciiSymbol(char c);
  bool _matchResponse(char expectedResponse);
  bool _matchResponse(char* expectedResponse);
  int _getBaudCode(unsigned int baudeRate);

 public:
  /**
   * @brief Construct a new instance of a RG15 sensor.
   *
   * @param serial refernce to the  serial port to use (usually Serial1 or
   * Serial2 on the senseBox)
   */
  RG15(HardwareSerial& serial);

  /**
   * @brief Initialize the sensor and communication.
   *
   * @param baudRate the baud rate for serial sensor communication
   * @param timeout timeout for sensor communication
   * @param attempts number of attempts for sensor communication
   * @param highResolution true for high resolution, false for low resolution
   * @param metricUnit 'm' for metric, 'i for imperial units
   * @return true if the sensor was initialized successfully, false if not
   */
  bool begin(int baudRate, unsigned long timeout, unsigned int attempts,
             bool highResolution, char unit);

  /**
   * @brief Initialize the sensor and communication with specific baude rate,
   * timeout, attempts and default polling mode, high resolution and metric
   * units.
   *
   * @param baudRate the baud rate for serial sensor communication
   * @param timeout timeout for sensor communication
   * @param attempts number of attempts for sensor communication
   * @return true if the sensor was initialized successfully, false if not
   */
  bool begin(unsigned int baudeRate, unsigned long timeout,
             unsigned int attempts);

  /**
   * @brief Initialize the sensor and communication with default parameters of
   * 9600 baud rate, 1000ms timeout, 5 attempts, polling mode, high resolution
   * and metric units.
   *
   * @return true if the sensor was initialized successfully, false if not
   */
  bool begin();

  /**
   * @brief Get all rain gauge data in polling mode and store it in the sensor
   * object
   *
   * @return true if the data was successfully retrieved, false if not
   */
  bool poll();

  /**
   * @brief Restarts the sensor.
   *
   * @return true if the sensor was restarted succesfully, false if not
   */
  bool restart();

  /**
   * @brief Change the baud rate of the sensor and reastablish communication.
   *
   * @param baudeRate the baude rate to change to, only 200, 2400, 4800, 9600,
   * 19200, 38400, 57600 are valid
   * @return true if the baude rate was changed correctly, false if not
   */
  bool changeBaudRate(unsigned int BaudeRate);

  /**
   * @brief Set the sensor to polling mode. (This is the only supported mode and
   * can be used as ping test.)
   *
   * @return true if the polling mode was set and acknowledged by the sensor ,
   * false if not
   */
  bool setPollingMode();

  /**
   * @brief Set high resolution
   *
   * @return true if the high resolution was set and acknowledged by the sensor,
   * false if not
   */
  bool setHighResolution();

  /**
   * @brief Set low resolution
   *
   * @return true if the low resolution was set and acknowledged by the sensor,
   * false if not
   */
  bool setLowResolution();

  /**
   * @brief Set the unit of the sensorto imperial
   *
   * @return true if matric unit was set and acknowledged by the sensor, false
   * if not
   */
  bool setImperialUnit();

  /**
   * @brief Set the unit of the sensor to metric
   *
   * @return true if inperial unit was set and acknowledged by the sensor, false
   * if not
   */
  bool setMetricUnit();

  /**
   * @brief Reset the total accumulated rainfall inside the sensor and update
   * the totalAcc variable
   *
   * @return true if the reset was succesfull and false if the reset was not
   * succesfull
   */
  bool resetTotalAccumulation();

  /**
   * @brief Get the accumulated rainfall since the last poll
   *
   * @return float The accumulated rainfall since the last poll
   */
  float getAccumulation();

  /**
   * @brief Get the accumulated rainfall in the current event
   *
   * @return float The accumulated rainfall in the current event
   */
  float getEventAccumulation();

  /**
   * @brief Get the total accumulated rainfall since the last reset
   *
   * @return float The total accumulated rainfall since the last reset
   */
  float getTotalAccumulation();

  /**
   * @brief Get the currunt rainfall intensity
   *
   * @return float The currunt rainfall intensity
   */
  float getRainfallIntensity();

  /**
   * @brief Get the latest error code for debugging purposes.
   *
   * @return 0 if no error happened yet,
   * @return 1 if serial does not exist,
   * @return 2 if serial could not write,
   * @return 3 if response is invalid,
   * @return 4 if response timed out,
   * @return 5 if baud rate is not supported,
   * @return 6 if parsing did not work,
   * @return 7 if unit does not match
   */
  int getErrorCode();

  /**
   * @brief Get the response buffer for debugging purposes.
   *
   * @return the response buffer as cstring
   */
  char* getResponseBuffer();
};

/**
 * TODO
 * Here is a list of unimplemented features that were unnecessary up to now:
 * - proper usage of continous mode
 * - only reading accumulation
 * - using the hardware switches
 * - stuff regarding the tipping bucket
 */
