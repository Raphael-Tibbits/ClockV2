#pragma once

#include "clock_peripherals.h"

#include <avr/interrupt.h>

typedef struct{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
}time;

typedef struct{
    uint8_t day_of_the_week;
    uint8_t day_of_the_month;
    uint8_t month;
    uint8_t year;
    uint32_t unix_s;
}date;

typedef struct{
    date today;
    time now;
}datetime;

extern volatile uint64_t miliseconds;

extern volatile uint8_t flag = 0;


