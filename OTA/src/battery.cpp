#include "battery.h"
#include "Arduino.h"
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int GetBatteryPercentage()
{

    float sensorValue = analogRead(A0);
    float voltage = (((sensorValue) / 1024.0) * RCONST + BAT_CALIBRATION); // 10 - 100k/10k. Later design use 300k/100k

    int bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100); // 2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage

    if (bat_percentage >= 100)
    {
        bat_percentage = 100;
    }
    if (bat_percentage <= 0)
    {
        bat_percentage = 1;
    }
    return bat_percentage;
}
