#include "RG15.hpp"

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

/**
 * private functions for robust serial communication to change sensor settings
 */
bool RG15::changeSettings(char setting) {
  if (cleanSerialStream())
    if (sendChar(setting))
      if (collectResponse())
        if (checkCharResponse(setting)) return true;  // success
  return false;                                       // fail
}

bool RG15::cleanSerialStream() {
  if (serial != nullptr) {
    while (serial->available()) serial->read();  // clean serial stream
    return true;                                 // success
  }
  latestErrorCode = 1;  // serial does not exist
  return false;         // fail
}

bool RG15::sendChar(char charCommand) {
  if (serial->write(charCommand) == 1)
    if (serial->write('\n') == 1) return true;  // success
  latestErrorCode = 2;                          // serial could not write
  return false;                                 // fail
}

bool RG15::collectResponse() {
  long sendTime = millis();
  int index = 0;
  while (millis() - sendTime < timeout) {
    if (serial->available()) {
      responseBuffer[index] = serial->read();

      // check fr line break
      if (responseBuffer[index++] == '\n') {
        responseBuffer[index] = '\0';  // null termination
        return true;                   // success
      }

      // check for size limit of response buffer
      if (index == sizeof(responseBuffer) - 1) {  // to long
        responseBuffer[index] = '\0';  // null termination for good measure
        latestErrorCode = 3;           // response is invalid
        return false;                  // fail
      }
    }
  }
  latestErrorCode = 4;  // response timed out
  return false;         // fail
}

bool RG15::checkCharResponse(char expectedChar) {
  if (strlen(responseBuffer) > 0)
    if (responseBuffer[0] == expectedChar) return true;  // success
  latestErrorCode = 3;                                   // response is invalid
  return false;                                          // fail
}

/**
 * additional private functions for robust serial communication
 */
bool RG15::checkResponse(char* expectedResponse) {
  // check size
  size_t expectedResponseSize = strlen(expectedResponse);
  size_t responseSize = strlen(responseBuffer);
  if (responseSize < expectedResponseSize) {
    latestErrorCode = 3;  // response is invalid
    return false;         // fail
  }

  // check single characters
  for (size_t i = 0; i < expectedResponseSize; i++) {
    if (responseBuffer[i] != expectedResponse[i]) {
      latestErrorCode = 3;  // response is invalid
      return false;         // fail
    }
  }
  return true;  // success
}

bool RG15::isValidBaudRate(unsigned int baudRate) {
  for (int baudCode = 0; baudCode < sizeof(baudeRateCodes); baudCode++)
    if (baudeRateCodes[baudCode] == baudRate) return true;  // success
  latestErrorCode = 5;  // baud rate is not supported
  return false;         // fail
}

/**
 * constructor
 */
RG15::RG15(HardwareSerial& serial) {
  this->serial = &serial;
  this->acc = 0;
  this->eventAcc = 0;
  this->totalAcc = 0;
  this->rInt = 0;
  this->latestErrorCode = 0;
}

/**
 * begin
 */
bool RG15::begin(int baudRate, unsigned long timeout, unsigned int attampts,
                 bool pollingMode, bool highResolution, bool metricUnit) {
  this->timeout = timeout;
  this->attempts = attempts;

  // baude rate
  if (!isValidBaudRate(baudRate)) return false;  // fail
  if (!cleanSerialStream()) return false;        // fail
  serial->begin(baudRate);

  // sensor settings
  bool success = true;
  success &= (setPollingMode() ? pollingMode : setContinousMode());
  success &= (setHighResolution() ? highResolution : setLowResolution());
  success &= (setMetricUnit() ? metricUnit : setImperialUnit());
  return success;
}
bool RG15::begin(unsigned int baudRate, unsigned long timeout,
                 unsigned int attempts) {
  begin(baudRate, timeout, attempts, true, true, true);
}
bool RG15::begin() { begin(9500, 1000, 5); }

/**
 * poll measurements
 */
bool RG15::poll() {
  for (int attempt = 0; attempt < attempts; attempt++) {
    if (cleanSerialStream())
      if (sendChar('r'))
        if (collectResponse()) {
          char newUnit;
          if (sscanf(responseBuffer,
                     "Acc %f %c %*s EventAcc %f %*s TotalAcc %f %*s RInt %f",
                     &acc, &newUnit, &eventAcc, &totalAcc, &rInt) != 5) {
            latestErrorCode = 3;  // response is invalid
            return false;         // fail
          }
          if (unit == newUnit) return true;  // success
          latestErrorCode = 6;               // response unit does not match
          return false;                      // fail
        }
  }
  return false;  // fail
}

/**
 * restart sensor
 */
bool RG15::restart() {
  for (int attempt = 0; attempt < attempts; attempt++) {
    if (cleanSerialStream())
      if (sendChar('k'))
        if (collectResponse())
          if (checkResponse("Device Restart")) return true;  // success
  }
  return false;  // fail
}

/**
 * change baud rate //TODO
 */
bool RG15::changeBaudeRate(unsigned int baudeRate) {
  for (int attempt = 0; attempt < attempts; attempt++) {
    if (!cleanSerialStream()) return false;  // fail

    if (serial->write('B') == 1)
      if (serial->write('\n') == 1) return true;  // success
    latestErrorCode = 2;                          // serial could not write
    return false;                                 // fail

    if (collectResponse())
      if (checkResponse("Device Restart")) return true;  // success
    return false;                                        // fail
  }
}

/**
 * polling settings
 */
bool RG15::setPollingMode() {
  for (int attempt = 0; attempt < attempts; attempt++)
    if (changeSettings('p')) {
      pollingMode = true;
      return true;
    }
  return false;
}
bool RG15::setContinousMode() {
  for (int attempt = 0; attempt < attempts; attempt++)
    if (changeSettings('c')) {
      pollingMode = false;
      return true;
    }
  return false;
}

/**
 * resolution settings
 */
bool RG15::setHighResolution() {
  for (int attempt = 0; attempt < attempts; attempt++)
    if (changeSettings('h')) return true;  // success
  return false;                            // fail
}
bool RG15::setLowResolution() {
  for (int attempt = 0; attempt < attempts; attempt++)
    if (changeSettings('h')) return true;  // success
  return false;                            // fail
}

/**
 * unit settings
 */
bool RG15::setImperialUnit() {
  for (int attempt = 0; attempt < attempts; attempt++)
    if (changeSettings('i')) {
      unit = 'i';
      return true;  // success
    }
  return false;  // fail
}
bool RG15::setMetricUnit() {
  for (int attempt = 0; attempt < attempts; attempt++)
    if (changeSettings('m')) {
      unit = 'm';
      return true;  // success
    }
  return false;  // fail
}

/**
 * reset total accumulation counter
 */
bool RG15::resetTotalAccumulation() {
  for (int attempt = 0; attempt < attempts; attempt++)
    if (cleanSerialStream())
      if (sendChar('o')) return true;  // success
  return false;                        // fail
}

/**
 * get measurements
 */
float RG15::getAccumulation() { return this->acc; }
float RG15::getEventAccumulation() { return this->eventAcc; }
float RG15::getTotalAccumulation() { return this->totalAcc; }
float RG15::getRainfallIntensity() { return this->rInt; }

/**
 * get latest error code
 */
int RG15::getLatestErrorCode() { return this->latestErrorCode; }
