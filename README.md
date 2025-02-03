## Overview

The **RG15 Arduino Library (RG15-Arduino)** provides robust communication with the [**RG15 Rain Gauge Sensor**](https://rainsensors.com/products/rg-15/) over a serial interface.

It was designed for use with the [senseBox microcontroller](https://sensebox.de/) and fixes problems with the [standard library](https://rainsensors.com/docs/rg-guides/rg-arduino/hydreon-arduino-library/) provided by hydreon and our [old version](https://github.com/sensebox/hydreon-rainsensor-library). This library is designed for accurate and reliable retrieval of rainfall data, supporting polling mode and essential sensor functionalities.

## Features

- Retrieves real-time rainfall data, including:
  - Accumulated rainfall since the last poll.
  - Accumulated rainfall for the current event.
  - Total accumulated rainfall since last reset.
  - Rainfall intensity.
- Supports metric (mm) and imperial (inches) units.
- Allows change of valid baud rates (1200 to 57600).
- Allows sensor reinitialization and polling mode.
- Provides error handling and debugging capabilities.
- Compatible with **Arduino-based microcontrollers**, including the [**senseBox**](https://sensebox.de/).

## Installation

1. Download or clone the repository.
2. Copy the **RG15** folder into your Arduino **libraries** directory:
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

| Method                                                     | Description                                                                      |
| ---------------------------------------------------------- | -------------------------------------------------------------------------------- |
| `bool begin(int baudRate, bool highResolution, char unit)` | Initializes the sensor with custom settings.                                     |
| `bool begin()`                                             | Initializes with default settings: **9600 baud, high resolution, metric units**. |
| `bool poll()`                                              | Retrieves the latest rain data from the sensor.                                  |
| `float getAccumulation()`                                  | Returns accumulated rainfall since last poll.                                    |
| `float getEventAccumulation()`                             | Returns accumulated rainfall in the current event.                               |
| `float getTotalAccumulation()`                             | Returns total accumulated rainfall since last reset.                             |
| `float getRainfallIntensity()`                             | Returns current rainfall intensity.                                              |
| `bool restart()`                                           | Restarts the RG15 sensor.                                                        |
| `bool setPollingMode()`                                    | Sets the sensor to polling mode.                                                 |
| `bool changeBaudRate(unsigned int baudRate)`               | Changes the communication baud rate.                                             |
| `bool setUnit(char unit)`                                  | Sets measurement units: `'m'` for metric, `'i'` for imperial.                    |
| `char getUnit()`                                           | Gets the current measurement unit.                                               |
| `int getErrorCode()`                                       | Returns the latest error code for debugging.                                     |
| `char* getResponseBuffer()`                                | Returns the last received response from the sensor.                              |

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

## Notes & Limitations

- **Continuous mode is not supported** (only polling mode is implemented).
- Sensor settings like **high/low resolution** and **unit selection** must be configured after initialization.
- Communication is handled using **HardwareSerial** (software serial is not recommended).

## References

- **RG15 Sensor Documentation:** [RG15 Manual](https://rainsensors.com/wp-content/uploads/sites/3/2020/07/rg-15_instructions_sw_1.000.pdf#page=2)

## License

This library is open-source and licensed under the **MIT License**.

## Authors

- **Paul Reichmuth** (@PaulReichmuth)
- **Björn Luig** (@BjoernLuig)
- **Inspiration from:** Jan-Patrick Bollow (@JBollow)
