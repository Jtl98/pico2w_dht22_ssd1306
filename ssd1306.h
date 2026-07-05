#ifndef SSD1306_H
#define SSD1306_H

#include "pico/stdlib.h"

void ssd1306_init(uint i2c_number, uint sda_gpio, uint scl_gpio);
void ssd1306_flash(uint i2c_number);

#endif
