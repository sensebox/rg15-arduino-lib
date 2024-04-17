#include "Arduino.h"
#include <string>

class RG15
{
private:
    const int _baud = 1200;
    std::string _dataIn = "";
    HardwareSerial &serial;
    bool _polling = true;
    void _clearDataIn();
public:
    float acc = 0;
    float eventAcc = 0;
    float totalAcc = 0;
    float rInt = 0;
    RG15(HardwareSerial &serial);
    ~RG15();
    bool begin(bool polling = true);
    bool doPoll();
    bool doPoll(float &acc, float &eventacc, float &totalacc, float &rInt);
    bool resetTotalAcc();
    bool reboot();
    float getAcc();
};
