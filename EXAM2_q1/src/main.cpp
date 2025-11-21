#include <Arduino.h>
#include <SPI.h>
#include "HSensor.h"


void setup() {
    // --- Initialize Serial ---
    Serial.begin(115200);
    delay(200);   // small pause for Serial to come up

    // --- Configure Chip Select pin ---
    pinMode(SENSOR_CS_PIN, OUTPUT);
    digitalWrite(SENSOR_CS_PIN, HIGH);   // deselect sensor

    // --- Initialize SPI ---
    SPI.begin();   // uses SCK, MOSI, MISO hardware pins

    // Optional: print confirmation
    Serial.println("SPI + Serial initialized.");
}


void loop()
{
    // Call UpdateSensor() once per loop
    int err = UpdateSensor();

    // If SPI transaction itself failed 
    if (err != 0) {
        Serial.println("NOT VALID: SPI/comm error");
        delay(1000);
        return;
    }

    // Read status flags
    int power = GetPowerStatus();
    int busy  = GetBusy();
    int mem   = GetMemStat();
    int math  = GetMathSat();

    // Check if everything is OK
    if (power == 1 && busy == 0 && mem == 0 && math == 0) {
        // All good → print pressure in mmHg
        int P = GetPressureData();
        Serial.print("Pressure (mmHg): ");
        Serial.println(P);
    } else {
        // Something wrong → print NOT VALID and reasons
        Serial.print("NOT VALID: ");

        if (power == 0) Serial.print("PowerOff ");
        if (busy  == 1) Serial.print("Busy ");
        if (mem   == 1) Serial.print("MemFail ");
        if (math  == 1) Serial.print("MathSat ");

        Serial.println();
    }

    // Wait 1 second before next reading
    delay(1000);
}


