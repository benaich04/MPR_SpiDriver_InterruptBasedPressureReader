#include "HSensor.h"

// --- Global variables (declared in header) ---
uint8_t  g_statusByte = 0;
uint32_t g_pressureRaw = 0;

// -------------------------------------------------------------
//  UpdateSensor()
//  Talks to the MPR sensor using SPI, reads status + 24-bit pressure
// -------------------------------------------------------------
int UpdateSensor(void)
{
    uint8_t b0, b1, b2, b3;

    // Begin SPI transaction
    SPI.beginTransaction(SPISettings(SENSOR_SPI_CLOCK, MSBFIRST, SENSOR_SPI_MODE));   // assuming 1MHz clock is safe

    // --- Step 1: Send Output Measurement Command 0xAA 0x00 0x00 ---
    digitalWrite(SENSOR_CS_PIN, LOW);
    SPI.transfer(0xAA);      // command byte
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    digitalWrite(SENSOR_CS_PIN, HIGH);

    // --- Step 2: Wait for conversion ---
    delay(5);   // datasheet gives 5 ms as typical conversion time (Option 2 in Table 19)

    // --- Step 3: Read 4 bytes: status + 3 data bytes ---
    digitalWrite(SENSOR_CS_PIN, LOW);
    b0 = SPI.transfer(0x00);   // status byte
    b1 = SPI.transfer(0x00);   // MSB of pressure
    b2 = SPI.transfer(0x00);   // mid
    b3 = SPI.transfer(0x00);   // LSB
    digitalWrite(SENSOR_CS_PIN, HIGH);

    SPI.endTransaction();

    // Store status byte
    g_statusByte = b0;

    // Assemble 24-bit pressure
    g_pressureRaw = ((uint32_t)b1 << 16) |
                    ((uint32_t)b2 << 8)  |
                     (uint32_t)b3;

    return 0;  // success
}

// -------------------------------------------------------------
//  Status accessors
// -------------------------------------------------------------
int GetPowerStatus(void)
{
    return (g_statusByte & STATUS_POWER_BIT) ? 1 : 0;
}

int GetBusy(void)
{
    return (g_statusByte & STATUS_BUSY_BIT) ? 1 : 0;
}

int GetMemStat(void)
{
    return (g_statusByte & STATUS_MEM_BIT) ? 1 : 0;
}

int GetMathSat(void)
{
    return (g_statusByte & STATUS_MATH_BIT) ? 1 : 0;
}

// -------------------------------------------------------------
//  Raw Pressure Value
// -------------------------------------------------------------
int GetPressureDataRaw(void)
{
    return (int)g_pressureRaw;
}

// -------------------------------------------------------------
//  GetPressureData()   (Transfer Function B for 0300YG)
// -------------------------------------------------------------
//
// From datasheet Table 9 (Transfer Function B):
//  Output counts range from 2.5% to 22.5% of 2^24 counts.
//  Physical range = 0 mmHg → 300 mmHg.    (Verify your exact part!)
// -------------------------------------------------------------
int GetPressureData(void)
{
    const float N_min = 0.025 * 16777216.0;   // ! 2.5% of 2^24
    const float N_max = 0.225 * 16777216.0;   // ! 22.5% of 2^24
    const float P_max = 300.0;                // ! 0300YG = 0–300 mmHg range

    float N = (float)g_pressureRaw;

    if (N < N_min) return 0;
    if (N > N_max) return (int)P_max;

    float P = ((N - N_min) / (N_max - N_min)) * P_max;
    return (int)P;
}
