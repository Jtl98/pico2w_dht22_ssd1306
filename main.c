#include <stdio.h>
#include "pico/stdlib.h"
#include "dht.h"

const uint DHT_GPIO = 16;

int main()
{
    stdio_init_all();
    gpio_init(DHT_GPIO);
    dht_init();

    while (true)
    {
        printf("reading dht\n");

        dht_reading reading;
        if (dht_read(DHT_GPIO, &reading))
            printf("humidity: %.1f%%, temperature: %.1fC\n", reading.humidity, reading.temperature);

        dht_wait();
    }
}
