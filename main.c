#include <stdio.h>
#include "pico/stdlib.h"
#include "dht22.h"

const uint DHT22_GPIO = 15;

int main()
{
    stdio_init_all();
    dht22_init(DHT22_GPIO);

    while (true)
    {
        printf("reading dht22\n");

        dht22_reading reading;
        if (dht22_read(DHT22_GPIO, &reading))
            printf("humidity: %.1f%%, temperature: %.1fC\n", reading.humidity, reading.temperature);

        dht22_wait();
    }
}
