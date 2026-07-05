#include <stdio.h>
#include "pico/stdlib.h"
#include "dht22.h"
#include "ssd1306.h"

const uint DHT_GPIO = 15;
const uint SSD1306_I2C_NUMBER = 0;
const uint SSD1306_SDA_GPIO = 15;
const uint SSD1306_SCL_GPIO = 17;

int main()
{
    stdio_init_all();
    dht_init(DHT_GPIO);
    ssd1306_init(SSD1306_I2C_NUMBER, SSD1306_SDA_GPIO, SSD1306_SCL_GPIO);

    while (true)
    {
        printf("reading dht\n");

        dht_reading reading;
        if (dht_read(DHT_GPIO, &reading))
            printf("humidity: %.1f%%, temperature: %.1fC\n", reading.humidity, reading.temperature);

        ssd1306_flash(SSD1306_I2C_NUMBER);

        dht_wait();
    }
}
