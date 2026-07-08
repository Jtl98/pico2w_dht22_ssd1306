#include <stdio.h>
#include "pico/stdlib.h"
#include "dht22.h"
#include "ssd1306.h"

const uint DHT22_GPIO = 15;
const uint SSD1306_I2C_NUMBER = 0;
const uint SSD1306_SDA_GPIO = 16;
const uint SSD1306_SCL_GPIO = 17;

int main()
{
    stdio_init_all();
    dht22_init(DHT22_GPIO);
    ssd1306_init(SSD1306_I2C_NUMBER, SSD1306_SDA_GPIO, SSD1306_SCL_GPIO);

    while (true)
    {
        printf("reading dht22\n");

        dht22_reading reading;
        if (dht22_read(DHT22_GPIO, &reading))
            printf("humidity: %.1f%%, temperature: %.1fC\n", reading.humidity, reading.temperature);

        ssd1306_flash(SSD1306_I2C_NUMBER);

        dht22_wait();
    }
}
