#include "ssd1306.h"
#include <string.h>
#include "hardware/i2c.h"

// datasheet: file:///./datasheets/SSD1306.pdf

// fast mode, see "I2C modes" table: https://en.wikipedia.org/wiki/I2C#Design
static const uint BAUD_RATE = 400 * 1000;

// 8.1.5 (a)
static const uint8_t ADDRESS = 0b0111100;
// 8.1.5.1/8.1.5.2
static const uint8_t COMMAND_CONTROL_BYTE = 0b10000000;
static const uint8_t DATA_CONTROL_BYTE = 0b01000000;

// 9-1 (table 1)
static const uint8_t ENTIRE_DISPLAY_ON_FOLLOW_RAM = 0xA4;
static const uint8_t ENTIRE_DISPLAY_ON_IGNORE_RAM = 0xA5;
static const uint8_t SET_DISPLAY_OFF = 0xAE;
static const uint8_t SET_DISPLAY_ON = 0xAF;

// 9-1 (table 3)
static const uint8_t SET_MEMORY_ADDRESSING_MODE = 0x20;
static const uint8_t HORIZONTAL_ADDRESSING_MODE = 0b00;

// Application Note - 2.1
static const uint8_t SET_CHARGE_PUMP_SETTING = 0x8D;
static const uint8_t ENABLE_CHARGE_PUMP = 0x14;

static void send_command(i2c_inst_t *i2c, const uint8_t command)
{
    const uint8_t buffer[2] = {COMMAND_CONTROL_BYTE, command};

    i2c_write_blocking(i2c, ADDRESS, buffer, 2, false);
}

static void send_data(i2c_inst_t *i2c, const uint8_t data[], const size_t data_size)
{
    const size_t buffer_size = data_size + 1;
    uint8_t buffer[buffer_size];

    buffer[0] = DATA_CONTROL_BYTE;
    memcpy(buffer + 1, data, data_size);

    i2c_write_blocking(i2c, ADDRESS, buffer, buffer_size, false);
}

void ssd1306_init(const uint i2c_number, const uint sda_gpio, const uint scl_gpio)
{
    i2c_inst_t *i2c = i2c_get_instance(i2c_number);
    i2c_init(i2c, BAUD_RATE);

    gpio_set_function(sda_gpio, GPIO_FUNC_I2C);
    gpio_set_function(scl_gpio, GPIO_FUNC_I2C);
    gpio_pull_up(sda_gpio);
    gpio_pull_up(scl_gpio);

    const uint8_t commands[] = {
        SET_DISPLAY_OFF,            // set display off before configuration
        SET_MEMORY_ADDRESSING_MODE, // allow changing memory addressing mode
        HORIZONTAL_ADDRESSING_MODE, // change memory addressing mode to horizontal
        SET_CHARGE_PUMP_SETTING,    // allow changing charge pump setting
        ENABLE_CHARGE_PUMP,         // change charge pump setting to enabled
        SET_DISPLAY_ON              // set display on after configuration
    };

    for (size_t i = 0; i < count_of(commands); i++)
        send_command(i2c, commands[i]);
}

void ssd1306_flash(const uint i2c_number)
{
    i2c_inst_t *i2c = i2c_get_instance(i2c_number);

    send_command(i2c, ENTIRE_DISPLAY_ON_IGNORE_RAM);
    sleep_ms(1000);
    send_command(i2c, ENTIRE_DISPLAY_ON_FOLLOW_RAM);
}
