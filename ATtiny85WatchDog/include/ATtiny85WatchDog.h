#ifndef ATTINY85WATCHDOG_H
#define ATTINY85WATCHDOG_H

#include <Arduino.h>
#include "neoPixStuff.h"

//-- https://github.com/GreyGnome/EnableInterrupt
#include <EnableInterrupt.h>

#define _PIN_NEOPIXELS        0       //-- GPIO-00 ==> DIL-6 ==> PB0
#define _PIN_DUMMY1           1       //-- GPIO-01 ==> DIL-5 ==> PB1
#define _PIN_HEARTBEAT        2       //-- GPIO-02 ==> DIL-7 ==> PB2  / INT0
#define _PIN_MASTER_EN        3       //-- GPIO-03 ==> DIL-2 ==> PB3
#define _PIN_RESET_MYSELF     4       //-- GPIO-04 ==> DIL-3 ==> PB4

#define _STARTUP_TIME     30000       //-- 30 seconden
#define _MAX_NO_HARTBEAT  90000       //-- 90 seconds
#define _LAST_WARNING     75000       //-- 15 seconds before reset
#define _FIRST_WARNING    60000       //-- 30 seconds before reset
#define _LAST_CHANGE       5000

#endif // ATTINY85WATCHDOG_H
