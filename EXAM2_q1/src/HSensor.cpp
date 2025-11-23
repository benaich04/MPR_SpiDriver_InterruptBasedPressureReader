#include "HSensor.h"

#include <Arduino.h>
#include <Wire.h>

static uint8_t  g_status  = 0;
static uint32_t g_press   = 0;
static uint8_t  g_i2c_init = 0;

/* ---- Internal helpers ---- */

static void i2c_init(void)
{
    if (g_i2c_init) return;
    Wire.begin();       // use default pins / speed
    g_i2c_init = 1;
}

/* ---- High-level driver API ---- */

int UpdateSensor(void)
{
    i2c_init();

    // Send Output Measurement Command: 0xAA 0x00 0x00
    Wire.beginTransmission(SENSOR_ADDRESS);
    Wire.write(0xAA);
    Wire.write(0x00);
    Wire.write(0x00);
    int txResult = Wire.endTransmission();  // 0 = success

    if (txResult != 0) {
        return HSENSOR_ERR_I2C;
    }

    // Fixed wait ≥ 5 ms for conversion
    delay(5);

    // Read Status + 24-bit pressure (4 bytes total)
    uint8_t n = Wire.requestFrom(SENSOR_ADDRESS, (uint8_t)4);
    if (n != 4) {
        return HSENSOR_ERR_I2C;
    }

    uint8_t status = Wire.read();
    uint8_t b2     = Wire.read();
    uint8_t b3     = Wire.read();
    uint8_t b4     = Wire.read();

    g_status = status;
    g_press  = ((uint32_t)b2 << 16) | ((uint32_t)b3 << 8) | b4;

    // Optional: treat MEM or MATHSAT as error
    if (GetMemStat() || GetMathSat())
        return HSENSOR_ERR_STATUS;

    return HSENSOR_OK;
}

/* Status helpers */

int GetPowerStatus(void)
{
    return (g_status >> 6) & 0x01;
}

int GetBusy(void)
{
    return (g_status >> 5) & 0x01;
}

int GetMemStat(void)
{
    return (g_status >> 2) & 0x01;
}

int GetMathSat(void)
{
    return g_status & 0x01;
}

/* Pressure conversion:
 * Transfer Function B: 2.5%–22.5% of 2^24 counts, 0–300 mmHg
 */

int GetPressureDataRaw(void)
{
    // int is 16-bit on AVR; raw is mainly for debugging
    return (int)(g_press & 0x7FFF);
}

int GetPressureData(void)
{
    const float OUT_MIN = 0.025f * 16777216.0f;  // 2.5% of 2^24
    const float OUT_MAX = 0.225f * 16777216.0f;  // 22.5% of 2^24
    const float P_MIN   = 0.0f;
    const float P_MAX   = 300.0f;

    float y = (float)g_press;

    if (y <= OUT_MIN) return (int)P_MIN;
    if (y >= OUT_MAX) return (int)P_MAX;

    float p = ( (y - OUT_MIN) * (P_MAX - P_MIN) ) /
              (OUT_MAX - OUT_MIN) + P_MIN;

    if (p < P_MIN) p = P_MIN;
    if (p > P_MAX) p = P_MAX;

    return (int)(p + 0.5f);   // rounded mmHg
}

/* For the interrupt-driven version (Question 2) */
void DataReady(void)
{
    // To be used in the INT0-based solution
}
