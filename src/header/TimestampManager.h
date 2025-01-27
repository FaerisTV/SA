#ifndef TIMESTAMP_MANAGER_H
#define TIMESTAMP_MANAGER_H

#include <Arduino.h>
#include <RTClib.h>

void setupTimestamp();

void TimestampTask(void* parameter);

String getFormattedTimestamp();

DateTime getCurrentTimestamp();

#endif
