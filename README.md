# MPR SPI Driver with Interrupt-Based Pressure Reader

A lightweight PlatformIO project implementing a full SPI driver for the Honeywell MPR pressure sensor, including both polling (Question 1) and interrupt-driven (Question 2) data acquisition. Designed for microcontrollers running Arduino-compatible frameworks.

## Overview

This repository contains two independent implementations:

* **EXAM2_q1**: SPI polling-based driver

  * Sends the 0xAA output command
  * Waits for conversion
  * Reads status byte and 24-bit pressure
  * Converts raw counts to mmHg using Transfer Function B

* **EXAM2_q2**: SPI + external interrupt (EOC pin)

  * Uses the sensor’s End-Of-Conversion pin
  * ISR sets a flag when new data is ready
  * Main loop reads and prints pressure only after interrupt

Both versions implement the same hardware abstraction functions in `HSensor.c` and `HSensor.h`.

## Features

* Full SPI transaction handling with CS control
* Status-byte decoding (power, busy, memory error, math saturation)
* Raw 24-bit pressure data extraction
* Conversion to mmHg for the MPR 0300YG variant
* Optional interrupt-based data readiness
* Modular C driver separated from PlatformIO main application

## File Structure

```
EXAM2_q1/
    src/
        HSensor.c
        HSensor.h
        main.cpp
    platformio.ini

EXAM2_q2/
    src/
        HSensor.c
        HSensor.h
        main.cpp
    platformio.ini
```

## Hardware Requirements

* Honeywell MPR Series digital pressure sensor
* MCU with hardware SPI (Arduino-compatible)
* EOC pin connected to external interrupt pin (only for Q2)

## How to Build

1. Open the project folder in PlatformIO.
2. Select the desired environment (Q1 or Q2).
3. Build and upload.

## Notes

* SPI mode 0 is required for the MPR series.
* Transfer Function B is used for the 0300YG pressure range (0–300 mmHg).
* The interrupt version requires stable wiring of the EOC line to INT0/INT1.

