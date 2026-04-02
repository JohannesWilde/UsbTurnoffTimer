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

## Schematic

### STC8G1K08 [SOP8]

Top view.

```
                     +----------+
  rotary encoder A --+ 1      8 +-- TM1637 I2C CLK
               VCC --+ 2      7 +-- TM1637 I2C Data
  rotary encoder B --+ 3      6 +-- MOSFET            [HIGH - conductive, LOW - blocking]   | Tx
               GND --+ 4      5 +-- push button       [pull-up, LOW when pushed]            | Rx
                     +----------+
```

### TM1637

TM1637-based 4 Digit 7-Segment Display Modul [bottom view].

```
  +---------------------------+
  +   +--+                    +-- GND
  +   +  +                    +-- VCC
  +   +  +                    +-- I2C Data
  +   +--+                    +-- I2C CLK
  +---------------------------+
```

### Rotary Encoder

Bottom view.

```
                +--------+
                +   __   +-- rotary encoder A
          GND --+  /  \  +
                + |    | +-- GND
  push button --+  \__/  +
                +        +-- rotary encoder B
                +--------+
```

### N-channel MOSFET

Top view.

```
           +-----+
  MOSFET --+     |-+
           +     | + GND2
     GND --+     |-+
           +-----+
```

Please note: This choice of an N-channel compared to a P-channel mosfet was probably what lead to this layout. It means that it is **not** *VCC* that is disconnected but *GND* and *GND2*.

### USB

In:

```
  o-- VCC
  o-- GND
```

Out:

```
  o-- VCC
  o-- GND2
```


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
  - If the cut off duration is 0s this mode will do nothing and show *idle* on screen.
  - Long button press: change to **ConfigureDelay**.


## Final Remarks

### Programming

For the actual programming of the STC8 microcontroller a USB to TTL converter is required.

Typicall the following connections have to be made [programmer on the left, STC8 on the right]:
- VCC <-> VCC
- GND <-> GND
- RX <-> TX
- TX <-> RX

For product-specific instructions please refer to the respective user manual.

The STC8 has a bootloader program which allows In Serial Programming [ISP]. This is active for a short time after a power up of the STC8 only [unless it is told otherwise]. So in order for STC ISP or stcgal to be able to program the STC8 one will have to powercycle it every time.

For this application the reset pin is configured as a digital IO pin [and needed as such]. So one can not use it. Instead one will have to disconnect VCC [or GND].

### HW Options

As a reference the HW Options as reported by STC ISP are shown here.

- IRC frequency: 24.017MHz
- Wakeup Timer frequency: 36.750KHz
- Oscillator gain is HIGH
- Do not detect the level of P3.2 and P3.3 next download
- Power-on reset, use the extra power-on delay
- RESET pin behaves as IO pin
- Reset while detect a Low-Voltage
- Thresh voltage level of the built-in LVD : 2.00 V
- Hardware do not enable Watch-Dog-Timer
- Watch-Dog-Timer pre-scalar : 256
- Watch-Dog-Timer stop count in idle mode
- Erase user EEPROM area at next download
- Do not control 485 at next download
- Do not check user password next download
- Reference voltage: 1185 mV (Range: 1100-1300mV)


The most important options are:
  - Select 24.000 MHz for "Input IRC Frequency".
  - Check "RESET pin used as I/O port" is checked.
