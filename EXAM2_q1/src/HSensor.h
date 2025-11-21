#ifndef HSENSOR_H
#define HSENSOR_H

#include <Arduino.h>
#include <SPI.h>

// Optional I2C address (exam asks for it; will not be used for SPI)
#define SENSOR_ADDRESS   0x18    // Default I2C address from datasheet;

// --- SPI configuration (for Q1, since you chose SPI) ---
#define SENSOR_CS_PIN    A5      // Chip Select pin for the sensor PF0 -> A5
#define SENSOR_SPI_MODE  SPI_MODE0   // Required: MPR uses SPI mode 0
#define SENSOR_SPI_CLOCK 1000000UL   // SPI clock (Hz); 1 MHz is a safe choice

// --- Status byte bit masks (from status-byte table) ---
#define STATUS_POWER_BIT   (1 << 6)  // Power indication
#define STATUS_BUSY_BIT    (1 << 5)  // Busy flag
#define STATUS_MEM_BIT     (1 << 2)  // Memory integrity error
#define STATUS_MATH_BIT    (1 << 0)  // Math saturation

// --- Global variables (defined in HSensor.c) ---
extern uint8_t  g_statusByte;   // Last status byte read
extern uint32_t g_pressureRaw;  // Last 24-bit raw pressure value

// --- Driver API for Question 1 ---
int UpdateSensor(void);

int GetPowerStatus(void);
int GetBusy(void);
int GetMemStat(void);
int GetMathSat(void);

int GetPressureDataRaw(void);
int GetPressureData(void);      // returns pressure in mmHg for 0300YG

#endif // HSENSOR_H
