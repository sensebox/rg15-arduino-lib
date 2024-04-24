#include "Arduino.h"
#include <string>

class RG15
{
private:
    const int _baud;
    std::string _dataIn;
    HardwareSerial &serial;
    bool _polling;
    bool _isMetric;
    
    bool _setUnit(bool isMetric);
    bool _setPolling();
    void _waitForSensorReady();
    float _acc;
    float _eventAcc;
    float _totalAcc;
    float _rInt;
    /**
     * @brief Variable to store the initialization error (0 = no error, 1 = reboot error, 2 = unit error, 3 = polling error, 4 = unknown error)
     * 
     */
    int _initErr;
public:
    RG15(HardwareSerial &serial, int baud);
    RG15(HardwareSerial &serial);
    ~RG15();
    bool begin(bool polling, bool isMetric);
    bool begin();
    bool doPoll();
    bool doPoll(float *acc, float *eventAcc, float *totalAcc, float *rInt);
    bool reboot();
    void resetTotalAcc();
    String getDataIn();
    int getInitErr();
    float getAcc();
    float getEventAcc();
    float getTotalAcc();
    float getRInt();

    void _clearDataIn();
};
