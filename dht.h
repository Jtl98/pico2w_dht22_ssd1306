#ifndef DHT_H
#define DHT_H

#include "pico/stdlib.h"

typedef struct
{
    float humidity;
    float temperature;
} dht_reading;

void dht_init();
bool dht_read(const uint gpio, dht_reading *reading);
void dht_wait();

#endif
