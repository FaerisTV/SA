#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>

void setupSensors();
void readSensorCO2Data();
void readSensorTandHData();
float getCurrentTemperature();
float getCurrentHumidity();
uint16_t getCurrentCO2();
void SensorCO2Task(void* parameter);
void SensorTandHTask(void* parameter);

#endif
