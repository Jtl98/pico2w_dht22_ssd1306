#ifndef SSD1306_H
#define SSD1306_H

#include "pico/stdlib.h"

void ssd1306_init(const uint i2c_number, const uint sda_gpio, const uint scl_gpio);
void ssd1306_display_text(const uint i2c_number, const char text[]);

#endif
