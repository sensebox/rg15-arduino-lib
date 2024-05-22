#include "RG15.h"
#include "senseBoxIO.h"

RG15 rg15(Serial1);

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    while(!Serial){}
    Serial.println("Starting RG15 example");
    if (rg15.begin())
    {
        Serial.println("RG15 sensor started");
    }
    else
    {
        Serial.println("RG15 sensor failed to start");
        Serial.print("Init error: ");
        Serial.println(rg15.getInitErr());
        Serial.print("Data in: ");
        Serial.println(rg15.getDataIn());
        while (1)
        {
            senseBoxIO.statusRed();
        }
    }
}


void loop()
{     
    float acc, evAcc, totAcc, rInt;
    Serial.println("Polling RG15 sensor");
    rg15.doPoll(&acc, &evAcc, &totAcc, &rInt);
    Serial.print("Accumulated rainfall: ");
    Serial.println(acc);
    Serial.print("Event accumulated rainfall: ");
    Serial.println(evAcc);
    Serial.print("Total accumulated rainfall: ");
    Serial.println(totAcc);
    Serial.print("Rain intensity: ");
    Serial.println(rInt);
}