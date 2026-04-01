# UsbTurnoffTimer  
  
CMake based project using SDCC and targeting an STC8G1K08A microcontroller.

## Binaries

STC ISP programming software (v6.91): 
- https://www.stcmicro.com/stc/stc8g1k08a.html

Or [`pip install stcgal`]:
- https://github.com/grigorig/stcgal

SDCC 4.5.0: 
- https://sdcc.sourceforge.net/

CMake & IDE (QtCreator Community):
- https://www.qt.io/

## Documentation

STC8G
- https://www.stcmicro.com/datasheet/STC8G-en.pdf

TM1637
- https://raw.githubusercontent.com/avishorp/TM1637/master/docs/TM1637_V2.4_EN.pdf

Rotary Encoder
- https://www.handsontec.com/dataspecs/module/Rotary%20Encoder.pdf

N-channel MOSFET
- https://www.alldatasheet.com/datasheet-pdf/download/1118399/UPI/QM3004D.html

## User Manual

### Overview

This program is intended to cut off the power to the secondary side in a USB power connection after a configurable time for a configurable duration.

After connecting the system to power, the controller will enter the configuration state for the delay. By turning the knob one can configure the delay in hours and minutes until the first time it cuts off the power.

By pressing the knob down one can change to the configuration of the power cut off duration. Here one can set the duration in seconds [0 s to 59 s] or minutes [1 m to 24 h].

Via a long press on the knob one changes to the countdown state, where it will then wait for the delay and turn off the USB power to the secondary side for the configured duration. Afterwards it will wait so that 24 hours after it cut off the power now, it will do so again.


In any mode after a timeout of 12.75 seconds without any user input the display will turn off. In order to wake it back up one will have to push or turn the knob.


### Modes

As an overview the states and transitions are as follows:

- **ConfigureDelay**
  - Set delay in minutes [0 m to 24 h] by turning the knob.
  - Short button press: change to **ConfigureTimeOff**.
  - Long button press: change to **Countdown**.

- **ConfigureTimeOff**
  - Set cut off duration in seconds [0 s to 59 s] or minutes [1 m to 24 h].
  - Short button press: change to **ConfigureDelay**.
  - Long button press: change to **Countdown**.

- **Countdown**
  - Wait for delay, cut off the power for cut off duration and repeat in 24 h periods.
  - Long button press: change to **ConfigureDelay**.
