#ifndef HSENSOR_H
#define HSENSOR_H

#include <stdint.h>

/* MPRLS0300YG: default 7-bit I2C address */
#define SENSOR_ADDRESS   0x18

/* EOC connected to INT0 (PD0) – for Question 2 */
#define INT_PIN          0

/* Return codes for UpdateSensor() */
#define HSENSOR_OK            0
#define HSENSOR_ERR_I2C      -1
#define HSENSOR_ERR_STATUS   -2
#define HSENSOR_ERR_TIMEOUT  -3

/* Trigger a measurement and read status + pressure */
int UpdateSensor(void);

/* Status bits from last measurement */
int GetPowerStatus(void);   /* 1 = powered, 0 = not powered */
int GetBusy(void);          /* 1 = busy,   0 = ready        */
int GetMemStat(void);       /* 1 = mem error, 0 = ok        */
int GetMathSat(void);       /* 1 = math saturation, 0 = ok  */

/* Pressure readings */
int GetPressureDataRaw(void);  /* raw 24-bit count (clipped to int) */
int GetPressureData(void);     /* pressure in mmHg (0–300)          */

/* For the interrupt-driven version (Question 2) */
void DataReady(void);

int IsDataReady(void);
void ClearDataReadyFlag(void);


#endif /* HSENSOR_H */
