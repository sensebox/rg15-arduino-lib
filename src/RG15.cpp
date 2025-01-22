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

// send lower case command letter
bool RG15Arduino::_sendCommandLetter(char a) {
  // check for existence of serial
  if (serial == nullptr) {
    return false;
  }

  // send command
  if (serial->write(a) == 1 && serial->write('\n') == 1) {
    // TODO delay?
    String response = serial->readStringUntil('\n');

    // check response
    if (response.length() > 0) {
      return response.charAt(0) == a;
    }
  }
  return false;  // failure because of unexpected response
}

// constructors
RG15::RG15(HardwareSerial &serial, int baud) : serial(serial), _baud(baud) {
  this->_acc = 0;
  this->_eventAcc = 0;
  this->_totalAcc = 0;
  this->_rInt = 0;
}
RG15::RG15(HardwareSerial &serial) : RG15(serial, 9600) {}

bool RG15::_readSensorResponse() {
  this->_clearDataIn();  // make sure the buffer is empty so we don't confuse
                         // old data with new data
  unsigned long start = millis();
  while (this->_dataIn.empty()) {
    while (this->serial.available()) {
      this->_dataIn += std::string(this->serial.readString().c_str());
    }
    if (millis() - start > 10000) {
      return false;  // if no response is received after 10 seconds, we assume
                     // the sensor is gone
    }
  }
  return true;
}

bool RG15::_setPolling() {
  this->serial.println("P");
  if (!this->_readSensorResponse()) {
    return false;
  }
  if (this->_dataIn.find(std::string("p")) == std::string::npos) {
    return false;
  } else {
    return true;
  }
}

bool RG15::_checkSensorReady() {
  if (!this->_readSensorResponse()) {
    return false;
  }
  if (this->_dataIn.find(std::string("RG-15")) == std::string::npos) {
    if (this->_dataIn.find(std::string("Reset")) == std::string::npos) {
      return false;
    } else {
      return true;
    }
  }
  return true;
}

bool RG15::begin(bool polling, bool isMetric, bool isHighRes) {
  this->serial.begin(this->_baud);
  if (!this->_checkSensorReady()) {
    if (!this->reboot()) {
      this->_initErr = 1;
      return false;
    }
  }
  if (!this->_setUnit(isMetric)) {
    this->_initErr = 2;
    return false;
  }
  if (polling) {
    if (!this->_setPolling()) {
      this->_initErr = 3;
      return false;
    } else {
      this->_polling = true;
    }
  }
  if (isHighRes) {
    if (!this->_setHighResolution()) {
      this->_initErr = 4;
      return false;
    }
  } else {
    if (!this->_setLowResolution()) {
      this->_initErr = 4;
      return false;
    }
  }
  this->_initErr = 0;
  return true;
}

bool RG15::begin() {
  this->serial.begin(this->_baud);
  this->_clearDataIn();
  if (!this->_checkSensorReady()) {
    if (!this->reboot()) {
      this->_initErr = 1;
      return false;
    }
  }
  if (!this->_setUnit(true)) {
    this->_initErr = 2;
    return false;
  }
  if (!this->_setPolling()) {
    this->_initErr = 3;
    return false;
  } else {
    this->_polling = true;
  }
  if (!this->_setHighResolution()) {
    this->_initErr = 4;
    return false;
  }
  return true;
}

bool RG15::reboot() {
  this->serial.println("K");
  if (!this->_readSensorResponse()) {
    return false;
  }
  if (this->_dataIn.find(std::string("RG-15")) == std::string::npos) {
    if (this->_dataIn.find(std::string("Reset")) == std::string::npos) {
      return false;
    } else {
      return true;
    }
  } else {
    return true;
  }
}

// settings
bool RG15Arduino::setHighResolution() { return _sendCommandLetter('h'); }
bool RG15Arduino::setLowResolution() { return _sendCommandLetter('l'); }
bool RG15Arduino::setMetric() { return _sendCommandLetter('m'); }
bool RG15Arduino::setImperial() { return _sendCommandLetter('i'); }
bool RG15Arduino::setPolling() { return _sendCommandLetter('p'); }
bool RG15Arduino::setContinuous() { return _sendCommandLetter('c'); }

// reset
void RG15::resetTotalAcc() { return _sendCommandLetter('o'); }

// get measurements
float RG15::getAcc() { return this->_acc; }
float RG15::getEventAcc() { return this->_eventAcc; }
float RG15::getTotalAcc() { return this->_totalAcc; }
float RG15::getRInt() { return this->_rInt; }
