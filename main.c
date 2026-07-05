#include <stdio.h>
#include "pico/stdlib.h"
#include "dht22.h"

const uint DHT_GPIO = 15;

int main()
{
    stdio_init_all();
    dht_init(DHT_GPIO);

    while (true)
    {
        printf("reading dht\n");

        dht_reading reading;
        if (dht_read(DHT_GPIO, &reading))
            printf("humidity: %.1f%%, temperature: %.1fC\n", reading.humidity, reading.temperature);

        dht_wait();
    }
}
