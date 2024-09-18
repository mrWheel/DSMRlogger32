# ATtiny85 Watch Dog for ESP8266/ESP32

## Overview

This project implements a watchdog functionality for any MCU using an ATtiny85 microcontroller. The watchdog monitors the main system (main MCU) for proper operation and can reset it if necessary.

## Hardware Configuration

The ATtiny85 is configured as follows:

```
ATMEL ATTINY85
                       +--\/--+
            RESET PB5 1|      |8 VCC
     <--[MCU_EN]  PB3 2|      |7 PB2  (INT0) <----- heartbeat
      [RESET_ME]  PB4 3|      |6 PB1  [x] 
                  GND 4|      |5 PB0  [NEO_LED] -> 
                       +------+
```

- PB0 (Pin 5): Connected to a NeoPixel LED for status indication
- PB2 (Pin 7): Receives heartbeat signals from the main system
- PB3 (Pin 2): Controls the main MCU's enable (reset) pin
- PB4 (Pin 3): Self-reset pin

## Programming

1.  First select [PROJECT TASKS] (the scary mask)
2.  Click on `set Fuses` (check that no errors occur)
3.  Now click on the [->] upload icon at the bottom of VSCode

## Functionality

The watchdog operates in several states, indicated by the NeoPixel LED:

1. **Power On**: Blue LED fades on and off for 30 seconds.
2. **Normal Operation**: Green LED heartbeat. Flashes bright when receiving heartbeat pulse from the main system.
3. **First Alarm**: Slow Red LED heartbeat if no heartbeats received for 60 seconds.
4. **Second Alarm**: Faster Red LED heartbeat if no heartbeats received for 75 seconds.
5. **Last Change**: Rapid Red LED blinking for 5 seconds before resetting the main system.
6. **Reset Master**: Bright White LED on while resetting the main system and waiting for it to restart.

## Key Features

- Uses interrupts to detect heartbeat signals from the main system.
- Implements a state machine for different levels of system monitoring.
- Utilizes a NeoPixel LED for visual status indication.
- Can reset both the main system MCU and itself if necessary.

## Configuration

The watchdog timing can be adjusted using the following constants:

- `_STARTUP_TIME`: 30 seconds
- `_MAX_NO_HARTBEAT`: 90 seconds (maximum time without a heartbeat before reset)
- `_LAST_WARNING`: 75 seconds (second alarm trigger)
- `_FIRST_WARNING`: 60 seconds (first alarm trigger)
- `_LAST_CHANGE`: 5 seconds (duration of last warning before reset)

## Usage

1. Connect the ATtiny85 to your main MCU system according to the pin configuration.
2. Program the ATtiny85 with this code.
3. Ensure your main system MCU sends regular heartbeat signals to the ATtiny85.
4. The watchdog will monitor your system and reset it if it becomes unresponsive.

## Dependencies

- EnableInterrupt library
- Custom NeoPixel library (neoPixStuff.h)

## License

This project is released under the MIT License. See the source code for full license text.