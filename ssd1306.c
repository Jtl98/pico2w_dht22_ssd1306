#include "ssd1306.h"
#include <string.h>
#include "hardware/i2c.h"

// datasheet: file:///./datasheets/SSD1306.pdf

typedef struct
{
    uint8_t start_column;
    uint8_t end_column;
    uint8_t start_page;
    uint8_t end_page;
    uint16_t size;
} data_info;

// fast mode, see "I2C modes" table: https://en.wikipedia.org/wiki/I2C#Design
static const uint BAUD_RATE = 400 * 1000;

// 8.1.5 (a)
static const uint8_t ADDRESS = 0b0111100;
// 8.1.5.1/8.1.5.2
static const uint8_t COMMAND_CONTROL_BYTE = 0b10000000;
static const uint8_t DATA_CONTROL_BYTE = 0b01000000;

// 8.7
static const uint8_t TOTAL_COLUMNS = 128;
static const uint8_t TOTAL_PAGES = 8;

// 9-1 (table 1)
static const uint8_t ENTIRE_DISPLAY_ON_FOLLOW_RAM = 0xA4;
static const uint8_t ENTIRE_DISPLAY_ON_IGNORE_RAM = 0xA5;
static const uint8_t SET_DISPLAY_OFF = 0xAE;
static const uint8_t SET_DISPLAY_ON = 0xAF;

// 9-1 (table 3)
static const uint8_t SET_MEMORY_ADDRESSING_MODE = 0x20;
static const uint8_t HORIZONTAL_ADDRESSING_MODE = 0b00;
static const uint8_t SET_COLUMN_ADDRESS = 0x21;
static const uint8_t SET_PAGE_ADDRESS = 0x22;

// 9-1 (table 4)
static const uint8_t SET_SEGMENT_REMAP_127 = 0xA1;
static const uint8_t SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED = 0xC8;

// Application Note - 2.1
static const uint8_t SET_CHARGE_PUMP_SETTING = 0x8D;
static const uint8_t ENABLE_CHARGE_PUMP = 0x14;

static const uint8_t FIRST_COLUMN = 0;
static const uint8_t LAST_COLUMN = TOTAL_COLUMNS - 1;
static const uint8_t FIRST_PAGE = 0;
static const uint8_t LAST_PAGE = TOTAL_PAGES - 1;

static void create_data_info(data_info *info, const uint8_t start_column, const uint8_t end_column, const uint8_t start_page, const uint8_t end_page)
{
    info->start_column = start_column;
    info->end_column = end_column;
    info->start_page = start_page;
    info->end_page = end_page;
    info->size = (end_column - start_column + 1) * (end_page - start_page + 1);
}

static void send_command(i2c_inst_t *i2c, const uint8_t command)
{
    const uint8_t buffer[2] = {COMMAND_CONTROL_BYTE, command};

    i2c_write_blocking(i2c, ADDRESS, buffer, 2, false);
}

static void send_commands(i2c_inst_t *i2c, const uint8_t commands[], const size_t commands_size)
{
    for (size_t i = 0; i < commands_size; i++)
        send_command(i2c, commands[i]);
}

static void send_data(i2c_inst_t *i2c, const uint8_t data[], const data_info *info)
{
    const uint16_t buffer_size = info->size + 1;
    uint8_t buffer[buffer_size];

    buffer[0] = DATA_CONTROL_BYTE;
    memcpy(buffer + 1, data, info->size);

    i2c_write_blocking(i2c, ADDRESS, buffer, buffer_size, false);
}

static void display_data(i2c_inst_t *i2c, const uint8_t data[], const data_info *info)
{
    const uint8_t commands[] = {
        SET_COLUMN_ADDRESS,
        info->start_column,
        info->end_column,
        SET_PAGE_ADDRESS,
        info->start_page,
        info->end_page};

    send_commands(i2c, commands, count_of(commands));
    send_data(i2c, data, info);
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
        SET_DISPLAY_OFF,                        // set display off before configuration
        SET_MEMORY_ADDRESSING_MODE,             // allow changing memory addressing mode
        HORIZONTAL_ADDRESSING_MODE,             // change memory addressing mode to horizontal
        SET_SEGMENT_REMAP_127,                  // change segment remap to flip the display horizontally
        SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED, // change COM output scan direction to flip the display vertically
        SET_CHARGE_PUMP_SETTING,                // allow changing charge pump setting
        ENABLE_CHARGE_PUMP,                     // change charge pump setting to enabled
        SET_DISPLAY_ON                          // set display on after configuration
    };

    send_commands(i2c, commands, count_of(commands));
}

void ssd1306_flash(const uint i2c_number)
{
    i2c_inst_t *i2c = i2c_get_instance(i2c_number);

    // blank entire display
    data_info info;
    create_data_info(&info, FIRST_COLUMN, LAST_COLUMN, FIRST_PAGE, LAST_PAGE);

    uint8_t data[info.size];
    memset(data, 0, info.size);

    display_data(i2c, data, &info);

    // flash entire display
    send_command(i2c, ENTIRE_DISPLAY_ON_IGNORE_RAM);
    sleep_ms(500);
    send_command(i2c, ENTIRE_DISPLAY_ON_FOLLOW_RAM);
    sleep_ms(500);

    const uint16_t half_size = info.size / 2;

    // light up top half of display
    memset(data, 255, half_size);
    memset(data + half_size, 0, half_size);

    display_data(i2c, data, &info);

    sleep_ms(500);

    // light up bottom half of display
    memset(data, 0, half_size);
    memset(data + half_size, 255, half_size);

    display_data(i2c, data, &info);
}
