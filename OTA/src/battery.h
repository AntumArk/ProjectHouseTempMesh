#pragma once
//https://how2electronics.com/iot-based-battery-status-monitoring-system-using-esp8266/
// Batter
const int batPin = 6;
#define BAT_CALIBRATION 0 // Check Battery voltage using multimeter & add/subtract the value
#define R1 100000
#define R2 10000
#define R1R2 R1+R2
#define RCONST R1R2/R2
int GetBatteryPercentage();