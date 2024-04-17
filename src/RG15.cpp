#include "RG15.hpp"
#include "Arduino.h"

RG15::RG15(HardwareSerial &serial) : serial(serial){
    this->_dataIn = "";
}

RG15::~RG15() = default;

void RG15::_clearDataIn(){
    this->_dataIn = "";
}

bool RG15::begin(bool polling){
    this->serial.begin(this->_baud);
    while(this->serial.available()){
        this->_dataIn += static_cast<char> (this->serial.read());
    }
    if (_dataIn.find("RG15") == std::string::npos){
        return false;
    }
    this->_clearDataIn();
    this->serial.println("M");
    while (this->serial.available())
    {
        this->_dataIn += static_cast<char> (this->serial.read());
    }
    if (_dataIn.find("m") == std::string::npos){
        return false;
    }
    this->_clearDataIn();
    if(polling){
        this->serial.println("P");
        while (this->serial.available())
        {
            this->_dataIn += static_cast<char> (this->serial.read());
        }
        if (_dataIn.find("p") == std::string::npos){
            return false;
        }
        else{
            this->_polling = true;
        }
    }
    
    return true;
}

bool RG15::doPoll(){
    this->_dataIn = "";
    if(this->_polling){
        this->serial.println("R");
        while (this->serial.available())
        {
            this->_dataIn += static_cast<char> (this->serial.read());
        }
        if (_dataIn.find("r") == std::string::npos){
            return false;
        }
        else{
            this->acc = std::stof(_dataIn.substr(1, 6));
            this->eventAcc = std::stof(_dataIn.substr(7, 6));
            this->totalAcc = std::stof(_dataIn.substr(13, 6));
            this->rInt = std::stof(_dataIn.substr(19, 6));
            return true;
        }
    }
    return false;
}

