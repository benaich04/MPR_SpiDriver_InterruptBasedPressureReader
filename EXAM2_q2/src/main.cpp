#include <Arduino.h>
#include <SPI.h>
#include "HSensor.h"

// Global flag set inside ISR when EOC (pad 8) goes HIGH
volatile bool g_dataReady = false;

// Interrupt Service Routine
void DataReady() {
    // Keep ISR extremely short — just set a flag
    g_dataReady = true;
}

void setup() {
    // --- Initialize Serial ---
    Serial.begin(115200);
    delay(200);

    // --- Configure Chip Select pin ---
    pinMode(SENSOR_CS_PIN, OUTPUT);
    digitalWrite(SENSOR_CS_PIN, HIGH);  // deselect sensor

    // --- Initialize SPI ---
    SPI.begin();   // SCK, MOSI, MISO configured automatically

    // --- Configure EOC interrupt pin ---
    pinMode(INT_PIN, INPUT);   // ! INPUT or INPUT_PULLUP depending on wiring

    // Attach interrupt to rising edge of EOC (pad 8)
    attachInterrupt(digitalPinToInterrupt(INT_PIN), DataReady, RISING);

    Serial.println("SPI + Serial + Interrupt initialized.");
}

void loop() {
    // Only perform a sensor read when the interrupt says data is ready
    if (g_dataReady) {
        g_dataReady = false;    // clear flag

        // Call UpdateSensor()
        int err = UpdateSensor();
        if (err != 0) {
            Serial.println("NOT VALID: SPI/comm error");
            return;
        }

        // Read status flags
        int power = GetPowerStatus();
        int busy  = GetBusy();
        int mem   = GetMemStat();
        int math  = GetMathSat();

        // Check if everything is OK
        if (power == 1 && busy == 0 && mem == 0 && math == 0) {
            int P = GetPressureData();
            Serial.print("Pressure (mmHg): ");
            Serial.println(P);
        } else {
            Serial.print("NOT VALID: ");
            if (power == 0) Serial.print("PowerOff ");
            if (busy  == 1) Serial.print("Busy ");
            if (mem   == 1) Serial.print("MemFail ");
            if (math  == 1) Serial.print("MathSat ");
            Serial.println();
        }
    }

}
