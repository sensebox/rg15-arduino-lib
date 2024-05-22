# rg15-arduino-lib
Library for Hydreon RG15 and arduino, made to fix problems with old hydreon lib and original lib.

## Usage

### Declare the object
To use the Library, a RG15 object needs to be created first, like this: 
```c++
RG15 rg15(Serial1)
```
The argument given here is the Serial Port the RG15 Sensor is connected to. The baudrate can also be specified, otherwise 9600 is assumed:
```c++
RG15 rg15(Serial1, 9600)
```

### Setup

In the setup() function, the sensor needs to be initialized using the begin() Method. There are two ways to do this:

```c++
rg15.begin() //this initializes the sensor using metric units, polling mode and high resolution. For ease of use, this is recommended.
```
or 
```c++
rg15.begin(true, true, true) //here, the arguments control the following: polling mode, metric/imperial units, high resolution. False always means a deviation from the default values, so (false, false, false) would initialize the sensor with imperial units in continuous mode and with low resolution. 
```

### Reading and handling sensor data
To better control the flow of data, the library offers the `doPoll()` method. This method requests new data from the sensor and stores it in two places. Every call of the `doPoll()` method updates the values stored in the `rg15`-Object. Alternatively, the data can also be extracted into variables in the sketch to be manipulated by it. This is accomplished like follows:
```c++
float acc, evAcc, totAcc, rInt; //create some variables to store data in
rg15.doPoll(&acc, &evAcc, &totAcc, &rInt); //pass the addresses of these variables as arguments into the doPoll() function. 
```
If `doPoll()` is called without arguments, the values can be obtained by calling one of the four getter methods:
```c++
float acc = rg15.getAcc();
float evAcc = rg15.getEventAcc();
float totAcc = rg15.getTotalAcc();
float rInt = rg15.getRInt();
```
**Warning:** These methods do not update the data inside the object and only retrieve stored data. To get the most recent data, `doPoll()` should be called first.

### Sensor Control
To ease the use, this library allows only minimal control over the sensors parameters outside of initialisation. Nonetheless, the following Methods are available to change the sensors behaviour and get information about errors that have occurred during initialisation:

#### `reboot()`
This method forces the sensor to reboot itself, checking if the reboot was successful and blocking the program while the sensor is rebooting itself. This is also called internally upon initialisation.

#### `resetTotalAcc()`
This method resets two values: 
1. The TotalAcc variable stored inside the Sensor Object (accessed via `getTotalAcc()`)
2. The sensors internal totalAcc-counter, stored inside the controller itself.
It checks whether the sensor has correctly purged its data by checking if the totalAcc returned from the sensor is actually 0, calling `doPoll` internally.

#### `getInitErr()`
This function returns an integer depending on what went wrong during initialization of the sensor. There are four different possible errors:
- `return = 1` The sensor failed to send it's boot header upon initialization and a second reboot attempt has failed (usually a wiring problem)
- `return = 2` The sensor failed to respond to the unit change command or responded incorrectly (i.e. confirmed metric units upon asking for imperial units)
- `return = 3` The sensor failed to respond to the polling  command or responded incorrectly (i.e. confirmed continuous operation upon asking for polling mode)
- `return = 4` The sensor failed to respond to the resolution change command or responded incorrectly (i.e. confirmed high resolution upon asking for low resolution)