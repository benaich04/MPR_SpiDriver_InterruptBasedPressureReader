#include <Arduino.h>
#include <avr/interrupt.h>
#include "HSensor.h"

// INT0 ISR: called when EOC pin toggles (conversion finished)
ISR(INT0_vect)
{
    DataReady();    // very short: just sets a flag inside HSensor.c
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for USB serial
    }

    Serial.println("EE4144 : Exam 2 - Q2: MPRLS0300YG (I2C + INT0)");

    // -------- Configure INT0 (PD0) as EOC input --------
    DDRD &= ~(1 << PD0);   // PD0 = input

    // Optional: enable pull-up if EOC is open-drain
    // PORTD |= (1 << PD0);

    // Trigger INT0 on rising edge: ISC01 = 1, ISC00 = 1
    EICRA |= (1 << ISC01) | (1 << ISC00);

    // Enable INT0
    EIMSK |= (1 << INT0);

    // Global interrupt enable
    sei();
}

void loop()
{
    // Wait until the interrupt tells us new data is ready
    if (!IsDataReady()) {
        // nothing to do yet
        return;
    }

    // Clear the flag before processing
    ClearDataReadyFlag();

    // Now do the I2C transaction and get the new pressure
    int err = UpdateSensor();

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

    // Just to avoid a crazy-fast print loop if EOC fires quickly
    delay(100);
}
