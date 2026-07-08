#ifndef DHT22_H
#define DHT22_H

#include "pico/stdlib.h"

typedef struct
{
    float humidity;
    float temperature;
} dht22_reading;

void dht22_init(const uint gpio);
bool dht22_read(const uint gpio, dht22_reading *reading);
void dht22_wait();

#endif
