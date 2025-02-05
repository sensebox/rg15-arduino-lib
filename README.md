## Overview

The **RG15 Arduino Library (RG15-Arduino)** provides robust communication with the [**RG15 Rain Gauge Sensor**](https://rainsensors.com/products/rg-15/) over a serial interface.

It was designed for use with the [senseBox microcontroller](https://sensebox.de/) and fixes problems with the [standard library](https://rainsensors.com/docs/rg-guides/rg-arduino/hydreon-arduino-library/) provided by hydreon and our [old version](https://github.com/sensebox/hydreon-rainsensor-library). This library is designed for accurate and reliable retrieval of rainfall data, supporting polling mode and essential sensor functionalities.

![image](https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg_sm.webp)

Image taken from [the producers website](https://rainsensors.com/products/rg-15/).

## Features

- Retrieves real-time rainfall data, including:
  - Accumulated rainfall since the last poll.
  - Accumulated rainfall for the current event.
  - Total accumulated rainfall since last reset.ist
  - Rainfall intensity.
- Supports metric (mm) and imperial (inches) units.
- Allows change of valid baud rates (1200 to 57600).
- Allows sensor reinitialization and polling mode.
- Provides error handling and debugging capabilities.
- Compatible with **Arduino-based microcontrollers**, including the [**senseBox**](https://sensebox.de/).

## Installation

1. Download or clone the repository.
2. Copy the **rg15-arduino-lib** folder into your Arduino **libraries** directory:
   ```
   Documents/Arduino/libraries/
   ```
3. Restart the Arduino IDE and include the library:
   ```cpp
   #include <RG15.h>
   ```

## Dependencies

This library requires the **Arduino core** and a hardware serial interface.

## Wiring Guide

| RG15 Pin on J2                  | Arduino Pin (Example) |
| ------------------------------- | --------------------- |
| VCC (Pin 2 for 5V+, 8 for 3.3V) | 5V                    |
| GND (Pin 1)                     | GND                   |
| OUT (Pin 3)                     | RX (e.g., Serial1 RX) |
| IN (Pin 5)                      | TX (e.g., Serial1 TX) |

## Usage

### Initialize the Sensor

```cpp
#include <RG15.h>

// Define serial port for sensor
// TODO

// Create RG15 object
RG15 rg51(Serial1);

void setup() {
    Serial.begin(9600); // Debug output

    // Initialize RG15 sensor with default settings
    if (rg15.begin()) {
        Serial.println("RG15 initialized successfully!");
    } else {
        Serial.println("Failed to initialize RG15!");
    }
}

void loop() {
    // Poll for rainfall data
    if (rg15.poll()) {
        Serial.print("Accumulated Rain: ");
        Serial.println(rg15.getAccumulation());
    } else {
        Serial.println("Failed to read RG15 data.");
    }
    delay(5000);
}
```

### Available Methods

# RG15 Arduino Library - Function Overview

## Class: `RG15`

| Function                                                                                                      | Return Type | Description                                                                                                                               |
| ------------------------------------------------------------------------------------------------------------- | ----------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| `RG15(HardwareSerial& serial, unsigned long cleanTime, unsigned long responseTime, unsigned int maxAttempts)` | Constructor | Creates an RG15 sensor instance with custom communication settings.                                                                       |
| `RG15(HardwareSerial& serial)`                                                                                | Constructor | Creates an RG15 sensor instance with default settings (cleanTime = 200 ms, responseTime = 1000 ms, maxAttempts = 5).                      |
| `bool begin(int baudRate, bool highResolution, char unit)`                                                    | `bool`      | Initializes the sensor with a given baud rate, resolution, and unit (`'m'` for metric, `'i'` for imperial). Returns `true` if successful. |
| `bool begin()`                                                                                                | `bool`      | Initializes the sensor with default settings (9600 baud, high resolution, metric). Returns `true` if successful.                          |
| `bool poll()`                                                                                                 | `bool`      | Retrieves the latest rain gauge data. Returns `true` if successful.                                                                       |
| `bool restart()`                                                                                              | `bool`      | Restarts the sensor. Returns `true` if successful.                                                                                        |
| `bool changeBaudRate(unsigned int baudRate)`                                                                  | `bool`      | Changes the baud rate to one of the supported values (1200, 2400, 4800, 9600, 19200, 38400, 57600). Returns `true` if successful.         |
| `bool setPollingMode()`                                                                                       | `bool`      | Sets the sensor to polling mode. Returns `true` if successful.                                                                            |
| `bool setHighResolution()`                                                                                    | `bool`      | Enables high-resolution mode. Returns `true` if successful.                                                                               |
| `bool setLowResolution()`                                                                                     | `bool`      | Enables low-resolution mode. Returns `true` if successful.                                                                                |
| `bool setUnit(char unit)`                                                                                     | `bool`      | Sets the sensor’s unit to metric (`'m'`) or imperial (`'i'`). Returns `true` if successful.                                               |
| `char getUnit()`                                                                                              | `char`      | Gets the current measurement unit (`'m'` for metric, `'i'` for imperial).                                                                 |
| `bool resetAccumulation()`                                                                                    | `bool`      | Resets accumulated rainfall (until next restart). Returns `true` if successful.                                                           |
| `float getAccumulation()`                                                                                     | `float`     | Retrieves the accumulated rainfall since the last poll.                                                                                   |
| `float getEventAccumulation()`                                                                                | `float`     | Retrieves the accumulated rainfall for the current event.                                                                                 |
| `float getTotalAccumulation()`                                                                                | `float`     | Retrieves the total accumulated rainfall since the last reset.                                                                            |
| `float getRainfallIntensity()`                                                                                | `float`     | Retrieves the current rainfall intensity.                                                                                                 |
| `int getErrorCode()`                                                                                          | `int`       | Gets the latest error code (0-7) for debugging.                                                                                           |
| `int getAttempts()`                                                                                           | `int`       | Retrieves the number of attempted communications.                                                                                         |
| `char* getResponseBuffer()`                                                                                   | `char*`     | Retrieves the response buffer for debugging.                                                                                              |

### Error Codes

| Code | Meaning                           |
| ---- | --------------------------------- |
| 0    | No error                          |
| 1    | Serial connection not established |
| 2    | Serial connection failed to write |
| 3    | Invalid response received         |
| 4    | Response timeout                  |
| 5    | Unsupported baud rate             |
| 6    | Data parsing failed               |
| 7    | Unit mismatch                     |

## Notes & Limitations## Notes:

- The library currently only supports polling mode.
- Continuous mode, hardware switches, and tipping bucket features are not yet implemented.
-

## References

- **RG15 Sensor Documentation:** [RG15 Manual](https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2)

## License

This library is open-source and licensed under the **MIT License**.

## Authors

- **Paul Reichmuth** (@PaulReichmuth)
- **Björn Luig** (@BjoernLuig)
- **Inspiration from:** Jan-Patrick Bollow (@JBollow)
