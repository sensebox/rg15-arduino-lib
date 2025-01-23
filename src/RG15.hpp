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
  HardwareSerial* serial;  // pointer onto serial port of the sensor
  unsigned int baudeRateCodes[7] = {
      1200,  2400,  4800, 9600,
      19200, 38400, 57600};  // valid baud rates indexed by baude codes (0-6)
  char responseBuffer[100];  // buffer to store the last sensor response
  unsigned long timeout;     // timeout for sensor responses
  unsigned int attempts;     // attempts at sensor communication
  bool pollingMode;          // if in polling mode or not (continous mode)
  char unit;  // unit as 'm' for metric in mm, 'i' for imperial in inch or '? if
              // unknown
  float acc;  // accumulated rainfall since last poll
  float eventAcc;       // accumulated rainfall in the current event
  float totalAcc;       // total accumulated rainfall since last reset
  float rInt;           // rainfall intensity
  int latestErrorCode;  // Code of the latest error for debugging purposes

  /**
   * @brief Change sensor setting with its character code.
   *
   * @param setting the character defining the setting
   * @return true if the setting was set and acknowledged by the sensor, false
   * if not
   */
  bool changeSettings(char setting);

  /**
   * @brief Clean the serial stream.
   *
   * @return true if the referenced serial object exists, false if not
   */
  bool cleanSerialStream();

  /**
   * @brief Send a single character into the stream followed by a '\n'.
   *
   * @param command the character to be send
   * @return true if the command was send correctly, false if not
   */
  bool sendChar(char charCommand);

  /**
   * @brief Collect all response up to '\n' of the stream.
   *
   * @return true if a valid response with '\n' was recieved, false if not
   */
  bool collectResponse();

  /**
   * @brief Check the latest response for its first character.
   *
   * @param expectedChar the expected first character
   * @return true if the first character of the response matches the expected
   * character, false if not
   */
  bool checkCharResponse(char expectedChar);

  /**
   * @brief Check the latest response for its first characters.
   *
   * @param expectedResponse the first characters of the expected response
   * @return true if the first characterr of the response matches the expected,
   * false if not expected
   */
  bool checkResponse(char* expectedResponse);

  /**
   * @brief Check if the given baud rate is supported by the sensor.
   *
   * @param baudRate the baud rate, only 200, 2400, 4800, 9600, 19200, 38400,
   * 57600 are valid
   * @return true if the baud rate is supported by the sensor, false if not
   */
  bool isValidBaudRate(unsigned int baudeRate);

 public:
  /**
   * @brief Construct a new instance of a RG15 sensor
   *
   * @param serial refernce to the  serial port to use (usually Serial1 or
   * Serial2)
   */
  RG15(HardwareSerial& serial);

  /**
   * @brief Initialize the sensor and communication.
   *
   * @param baudRate the baud rate for serial sensor communication
   * @param timeout timeout for sensor communication
   * @param attempts number of attempts for sensor communication
   * @param pollingMode true for polling mode, false for continous mode
   * @param highResolution true for high resolution, false for low resolution
   * @param metricUnit true for metric, false for imperial
   * @return true if the sensor was initialized successfully, false if not
   */
  bool begin(int baudRate, unsigned long timeout, unsigned int attampts,
             bool pollingMode, bool highResolution, bool metricUnit);

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
   * @return true if the polling mode was set and acknowledged by the sensor,
   * false if not
   */
  bool restart();

  /**
   * @brief Chnage the
   *
   * @param baudeRate the baude rate to change to, only 200, 2400, 4800, 9600,
   * 19200, 38400, 57600 are valid
   * @return true if the baude rate was changed correctly, false if not
   */
  bool changeBaudeRate(unsigned int BaudeRate);

  /**
   * @brief Set the sensor to polling mode
   *
   * @return true if the polling mode was set and acknowledged by the sensor ,
   * false if not
   */
  bool setPollingMode();

  /**
   * @brief Set the sensor to continuous mode
   *
   * @return true if the continuous mode was set and acknowledged by the sensor,
   * false if not
   */
  bool setContinousMode();

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
   * @brief Get the latest error code for debugging purposes
   *
   * @return 0 if no error happened yet,
   * @return 1 if serial does not exist,
   * @return 2 if serial could not write,
   * @return 3 if response is invalid,
   * @return 4 if response timed out,
   * @return 5 if baud rate is not supported,
   * @return 6 if parsing did not work,
   * @return 7 if response unit does not match
   */
  int getLatestErrorCode();
};

/**
 * TODO
 * Here is a list of unimplemented features:
 * - proper usage of continous mode (unneccessary)
 * - only reading accumulation (uneccessary, as done for all measurements at
 * ones)
 * - stuff regarding the tipping bucket
 */
