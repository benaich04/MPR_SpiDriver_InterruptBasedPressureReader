#include <Arduino.h>
#include "HSensor.h"

void setup()
{
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for USB serial (on boards like Leonardo/Micro)
    }

    Serial.println("EE4144  Exam 2 - Q1: MPRLS0300YG (I2C, Wire library)");
}

void loop()
{
    int err = UpdateSensor();   // perform one measurement

    if (err != HSENSOR_OK) {
        Serial.print("NOT VALID: I2C/comm error, code = ");
        Serial.println(err);
        delay(1000);
        return;
    }

    int power = GetPowerStatus();
    int busy  = GetBusy();
    int mem   = GetMemStat();
    int math  = GetMathSat();

    if (power == 1 && busy == 0 && mem == 0 && math == 0) {
        int pressure_mmHg = GetPressureData();
        Serial.print("Pressure: ");
        Serial.print(pressure_mmHg);
        Serial.println(" mmHg");
    } else {
        Serial.print("NOT VALID: ");
        if (power == 0) Serial.print("[Power OFF] ");
        if (busy  == 1) Serial.print("[Busy] ");
        if (mem   == 1) Serial.print("[Mem Error] ");
        if (math  == 1) Serial.print("[Math Sat] ");
        Serial.println();
    }

    delay(1000);   // update once per second
}
