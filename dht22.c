#include "dht22.h"
#include <stdio.h>

// datasheet: file:///./datasheets/DHT22.pdf

static const bool HIGH = true;
static const bool LOW = false;

static void wait_until(const uint gpio, const bool state)
{
    while (gpio_get(gpio) != state)
        sleep_us(1);
}

// 6.2 (step 1): "MCU send out start signal to DHT22"
static void send_start_signal(const uint gpio)
{
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 0);
    sleep_ms(20);
    gpio_set_dir(gpio, GPIO_IN);

    // wait for external pull-up resistor
    wait_until(gpio, HIGH);
}

// 6.2 (step 2): "DHT22 send response signal to MCU"
static void wait_for_response(const uint gpio)
{
    // 6.2 (step 1): "then MCU will wait 20-40us for DHT22's response."
    wait_until(gpio, LOW);

    // 6.2 (step 2): "DHT22 will send out low-voltage-level signal and this signal last 80us"
    wait_until(gpio, HIGH);

    // 6.2 (step 2): "then program of DHT22 transform data-bus's voltage level from low to high level and last 80us"
    wait_until(gpio, LOW);
}

// 6.2 (step 3): "DHT22 send data to MCU"
static void read_data(const uint gpio, uint8_t data[5])
{
    // 6.2: "DHT22 will send response signal of 40-bit data"
    for (uint i = 0; i < 40; i++)
    {
        // 6.2 (step 3): "every bit's transmission begin with low-voltage-level that last 50us"
        wait_until(gpio, HIGH);

        // 6.2 (step 3): "the following high-voltage-level signal's length decide the bit is "1" or "0"."
        const uint64_t start_time = time_us_64();

        wait_until(gpio, LOW);

        const uint64_t duration = time_us_64() - start_time;

        data[i / 8] <<= 1;

        // 26-28us = 0, 70us = 1
        if (duration > 50)
            data[i / 8] |= 1;
    }
}

static bool parse_data(uint8_t data[5], dht22_reading *reading)
{
    // 6.2: "check-sum should be the last 8 bit of "8 bit integral RH data+8 bit decimal RH data+8 bit integral T data+8 bit decimal T data"."
    const uint8_t checksum = ((data[0] + data[1] + data[2] + data[3]) & 0xFF);
    if (data[4] != checksum)
    {
        printf("checksum error: %d != %d\n", data[4], checksum);
        return false;
    }

    // from https://github.com/raspberrypi/pico-examples/blob/4c3a3dc0196dd426fddd709616d0da984e027bab/gpio/dht_sensor/dht.c#L78-L88
    reading->humidity = (float)((data[0] << 8) + data[1]) / 10;
    if (reading->humidity > 100)
    {
        reading->humidity = data[0];
    }
    reading->temperature = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
    if (reading->temperature > 125)
    {
        reading->temperature = data[2];
    }
    if (data[2] & 0x80)
    {
        reading->temperature = -reading->temperature;
    }

    return true;
}

void dht22_init(const uint gpio)
{
    gpio_init(gpio);

    // 6.1: "When power is supplied to sensor, don't send any instruction to the sensor within one second to pass unstable status."
    sleep_ms(1000);
}

bool dht22_read(const uint gpio, dht22_reading *reading)
{
    send_start_signal(gpio);

    wait_for_response(gpio);

    // 6.2: "DATA=8 bit integral RH data+8 bit decimal RH data+8 bit integral T data+8 bit decimal T data+8 bit check-sum"
    uint8_t data[5] = {0};
    read_data(gpio, data);

    return parse_data(data, reading);
}

void dht22_wait()
{
    // 7: "Collecting period should be : >2 second."
    sleep_ms(3000);
}
