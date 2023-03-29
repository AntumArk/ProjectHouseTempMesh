# Introduction

This document is for testing all of the assembled V2 features. 

# Required equipment
- 3.3-4.2V current limited PSU
- Multimeter
- Prepared DS1820S sensor with audio jack
- Thermometer
- MQTT broker
- Wifi network
- USB-C 5V PSU
- USB power meter

# Testpoints
- TP1 - Battery ADC voltage divider (470k 100k)
- TP2 - VCC 3.3V nominal (3-4.2V possible)
- TP3 - GND
- TP4 - VBUS 5V
- TP5 - Reset
- TP6 - Factory reset, GPIO13, pulled up
- TP7 - One wire DQ, data line
- TP8 - RX
- TP9 - TX
- JP2 - Deepsleep, shorts GPIO16 with reset
- U5 - select between USB and battery

# Test procedure
1. TP2 to TP3 does not short
2. TP4 to TP3 does not short
3. BAT+ (square pad) to BAT- does not short
4. Inspect USB C connector solder joints. No shorts, good solder.
5. PowerOn
    5.1 Select battery mode by soldering U5. 
    5.2 Set PSU to 4V, 100mA limit
    5.3 Connect PSU to board battery connector
    5.4 Observe ESP led blink.
    5.5 Current <100mA
    5.6 Disconnect PSU
    5.7 Select USB mode
    5.8 Through power meter connect board via USB PSU
    5.9 Repeat 5.4
    5.10 Repeat 5.5
    5.11 Repeat 5.6 
6. Programming(either using tx, rx, or programming adapter by Kaunas Makerspace)
    6.1 Board can be programmed with blink code
    6.2 Board communicates through serial
    6.3 Board can be programmed with firmware
7. Program features
    7.1 Keep programmer on the board.
    7.2 Device appears as hotspot with unique code
    7.3 External device (android or PC) can connect to hotspot
    7.4 Login page is prompted in website
    7.5 Can set Wifi settings
    7.6 Can set MQTT broker settings
    7.7 Can assign topic to device
    7.8 Save settings
    7.9 Device should connect to provided wifi
    7.10 Device should connect to provided MQTT
    7.11 Device should start publishing to provided topic
    7.12 Device publishes temperature equal to external thermometer
    7.13 Device publishes battery voltage (can be arbritary)
    7.14 Remove programmer
    7.15 Repeat 7.9-7.13 in USB mode
    7.16 Repeat 7.9-7.13 in Battery mode. 7.13 should show PSU voltage.
8. Extra features
    8.1 Pulling T6 low, will make board to return to initial state, where it acts like hotspot, ready to configured again.
    8.2 Jumping JP2, will enable deepsleep feature. Custom firmware is needed though.
    8.3 Charging protection (BAT MODE)
        8.4 Connect U6 VCC_Charge to Li-On charger 5V. 
        8.5 Connect charger. Hope for the best. Idk how to set. 