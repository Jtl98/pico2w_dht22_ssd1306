#include <stdio.h>
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();

    while (true)
    {
        printf("hello\n");
        sleep_ms(1000);
    }
}
