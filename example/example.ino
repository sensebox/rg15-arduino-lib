#include "RG15.hpp"
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
    if(!rg15.doPoll())
    {
        Serial.println("Polling failed");
    }
    else{
    Serial.print("Polling successful");
    }
    delay(1000);
    Serial.println("New Loop");
}