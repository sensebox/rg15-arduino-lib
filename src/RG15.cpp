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

bool RG15::_changeSettings(char settingCode) {
  settingCode = tolower(settingCode);
  if (!_prepSerial()) return false;              // fail
  if (!_sendCommand(settingCode)) return false;  // fail
  if (!_collectResponse()) return false;         // fail
  return (_matchResponse(settingCode));          // fail or success
}

bool RG15::_prepSerial() {
  if (_serial == nullptr) {
    _errorCode = 1;  // serial does not exist
    return false;    // fail
  } else {
    while (_serial->available()) _serial->read();  // clean
    return true;                                   // success
  }
}

bool RG15::_sendCommand(char command) {
  if (_serial->write(command) == 1 && _serial->write('\n') == 1) {
    return true;  // success
  } else {
    _errorCode = 2;  // serial could not write
    return false;    // fail
  }
}

bool RG15::_sendCommand(char* command) {
  if (_serial->write(command) == strlen(command) && _serial->write('\n') == 1) {
    return true;  // success
  } else {
    _errorCode = 2;  // serial could not write
    return false;    // fail
  }
}

bool RG15::_collectResponse() {
  delay(1000);  // TODO
  long sendTime = millis();
  int index = 0;
  char c;
  while (millis() - sendTime < _timeout) {  // ensure timeout
    if (_serial->available()) {
      // read valid ascii
      _responseBuffer[index] = _serial->read();
      // if (!_isAsciiSymbol(_responseBuffer[index])) continue;  // skip

      // check for line break
      if (_responseBuffer[index] == '\n') {
        _responseBuffer[++index] = '\0';  // ensure null termination
        return true;                      // success
      }

      // increase index and check for size limit of response buffer
      index++;
      if (index == sizeof(_responseBuffer) - 1) {
        _responseBuffer[index] = '\0';  // ensure null termination
        _errorCode = 30;                // response is invalid
        return false;                   // fail
      }
    }
  }
  _responseBuffer[index] = '\0';  // ensure null termination
  _errorCode = 40;                // response timed out
  return false;                   // fail
}

bool RG15::_isAsciiSymbol(char c) { return (c >= 32 && c <= 126); }

bool RG15::_matchResponse(char expectedResponse) {
  if (strlen(_responseBuffer) > 0 && _responseBuffer[0] == expectedResponse) {
    return true;  // success
  } else {
    _errorCode = 3;  // response is invalid
    return false;    // fail
  }
}

bool RG15::_matchResponse(char* expectedResponse) {
  if (strcmp(_responseBuffer, expectedResponse) == 0) {
    return true;  // success
  } else {
    _errorCode = 3;  // response is invalid
    return false;    // fail
  }
}

int RG15::_getBaudCode(unsigned int baudRate) {
  for (int baudCode = 0; baudCode < sizeof(_baudRates); baudCode++)
    if (_baudRates[baudCode] == baudRate) return baudCode;  // success
  _errorCode = 5;  // baud rate is not supported
  return -1;       // unvalid baud aret
}

RG15::RG15(HardwareSerial& serial) {
  _serial = &serial;
  _acc = 0;
  _eventAcc = 0;
  _totalAcc = 0;
  _rInt = 0;
  _errorCode = 0;
}

bool RG15::begin(int baudRate, unsigned long timeout, unsigned int attempts,
                 bool highResolution, char unit) {
  _timeout = timeout;
  _attempts = attempts;

  // start serial
  if (_getBaudCode(baudRate) == -1) return false;  // fail
  _serial->begin(baudRate);
  if (!_prepSerial()) return false;  // fail
  // sensor settings
  bool success = setPollingMode();  // only polling mode supported
                                    //   if (highResolution) {
                                    //     success &= setHighResolution();
                                    //   } else {
                                    //     success &= setLowResolution();
                                    //   }
                                    //   if (unit == 'm') {
                                    //     success &= setMetricUnit();
                                    //   } else if (unit == 'i') {
                                    //     success &= setImperialUnit();
                                    //   } else {
  //     _errorCode = 6;  // unit does not match
  //     return false;    // fail
  //   }
  return success;  // fail or success
}

bool RG15::begin(unsigned int baudRate, unsigned long timeout,
                 unsigned int attempts) {
  begin(baudRate, timeout, attempts, true, 'm');
}

bool RG15::begin() { begin(9500, 1000, 5); }

bool RG15::poll() {
  // strings for float conversion since sscanf does not support this in arduino
  char accBuffer[9];  // -> "xxxxx.xx" mm -> approx 30 years of rain
  char eventAccBuffer[9];
  char totalAccBuffer[9];
  char rIntBuffer[9];
  char unitBuffer[4];  // -> "mm," or "in,"

  // attempts
  for (int attempt = 0; attempt < _attempts; attempt++) {
    if (!_prepSerial()) continue;      // failed attempt
    if (!_sendCommand('r')) continue;  // failed attempt
    if (!_collectResponse())
      ;  // failed attempt TODO

    // parse response
    if (sscanf(_responseBuffer,
               "Acc %8s %s EventAcc %8s %*s TotalAcc %8s %*s RInt %8s",
               accBuffer, unitBuffer, eventAccBuffer, totalAccBuffer,
               rIntBuffer) == 5) {
      _acc = atof(accBuffer);
      _eventAcc = atof(eventAccBuffer);
      _totalAcc = atof(totalAccBuffer);
      _rInt = atof(rIntBuffer);

      // check unit
      if (unitBuffer[0] == 'm') {  // TODO check after begin() works
        return true;               // success
      } else {
        _errorCode = 6;  // unit does not match
        continue;        // failed attempt
      }
    } else {
      _errorCode = 3;  // response is invalid
      continue;        // failed attempt
    }
  }
  return false;  // failed all attempts
}

bool RG15::restart() {
  for (int attempt = 0; attempt < _attempts; attempt++) {
    if (!_prepSerial()) continue;                     // failed attempt
    if (!_sendCommand('k')) continue;                 // failed attempt
    if (!_matchResponse("Device Restart")) continue;  // failed attempt
    return true;                                      // success
  }
  return false;  // failed all attempts
}

bool RG15::changeBaudRate(unsigned int baudRate) {
  char baudCommand[8];
  for (int attempt = 0; attempt < _attempts; attempt++) {
    // get baud code for baud rate
    int baudCode = _getBaudCode(baudRate);
    if (baudCode == -1) continue;  // failed attempt

    // send baud command (here baud code)
    sprintf(baudCommand, "b %f", baudCode);
    if (!_prepSerial()) continue;  // failed attempt
    if (!_sendCommand(baudCommand)) continue;

    // check response (here baud rate)
    sprintf(baudCommand, "b %f", baudRate);
    if (!_collectResponse()) continue;           // failed attempt
    if (!_matchResponse(baudCommand)) continue;  // failed attempt

    // change own baud rate
    _serial->begin(baudRate);
    return true;  // success
  }
  return false;  // failed all attempts
}

bool RG15::setPollingMode() {
  for (int attempt = 0; attempt < _attempts; attempt++) {
    if (!_changeSettings('p')) continue;
    ;             // failed attempt
    return true;  // success
  }
  return false;  // all attempts failed
}

bool RG15::setHighResolution() {
  for (int attempt = 0; attempt < _attempts; attempt++) {
    if (!_changeSettings('h')) continue;  // failed attempt
    return true;                          // success
  }
  return false;  // all attempts failed
}

bool RG15::setLowResolution() {
  for (int attempt = 0; attempt < _attempts; attempt++) {
    if (!_changeSettings('l')) continue;  // failed attempt
    return true;                          // success
  }
  return false;  // all attempts failed
}

bool RG15::setImperialUnit() {
  for (int attempt = 0; attempt < _attempts; attempt++) {
    if (!_changeSettings('i')) continue;  // failed attempt
    _unit = 'i';
    return true;  // success
  }
  return false;  // all attempts failed
}

bool RG15::setMetricUnit() {
  for (int attempt = 0; attempt < _attempts; attempt++) {
    if (!_changeSettings('m')) continue;  // failed attempt
    _unit = 'm';
    return true;  // success
  }
  return false;  // all attempts failed
}

bool RG15::resetTotalAccumulation() {
  for (int attempt = 0; attempt < _attempts; attempt++)
    if (_sendCommand('o')) return true;  // success
  return false;                          // all attempts failed
}

float RG15::getAccumulation() { return _acc; }

float RG15::getEventAccumulation() { return _eventAcc; }

float RG15::getTotalAccumulation() { return _totalAcc; }

float RG15::getRainfallIntensity() { return _rInt; }

int RG15::getErrorCode() { return _errorCode; }

char* RG15::getResponseBuffer() { return _responseBuffer; }
