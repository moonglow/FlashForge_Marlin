/******************************************************************************
 * Additional info
 * ****************************************************************************
 * Changelog:
 * - v. 2.0 Optimize memory usage. Option to use binary search.
 *   2016-01-09 by Joakim Soderberg joakim.soderberg@gmail.com
 * - v. 1.0 Initial release)
 *   2014-04-24 by Albertas Mickėnas mic@wemakethings.net
 *
 * ****************************************************************************
 * Bugs, feedback, questions and modifications can be posted on the github page
 * on https://github.com/Miceuz/k-thermocouple-lib/
 * ****************************************************************************
 * - LICENSE -
 * GNU GPL v2 (http://www.gnu.org/licenses/gpl.txt)
 * This program is free software. You can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 * ****************************************************************************
 */

#include <stdio.h>
#include "thermocouple.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#undef KTHERM_WITH_PROGMEM
#endif

#ifdef KTHERM_TEST
#include <string.h>
#endif

/*
 * Values for this table should be multiplied with 1000 when used.
 * If we store the table as that instead, we need to use unsigned long
 * For example it would need (on an ATmega328):
 *    66 * 4 = 264 bytes
 * instead of:
 *    66 * 2 = 132 bytes
 */
#define TEMP_TABLE_SCALE 1000
static const unsigned int temp_table[]
    #ifdef KTHERM_WITH_PROGMEM
    PROGMEM
    #endif
    = {
    0,
    10,
    20,
    30,
    40,
    50,
    60,
    79,
    98,
    116,
    134,
    139,
    155,
    172,
    193,
    212,
    231,
    250,
    269,
    288,
    307,
    326,
    345,
    364,
    383,
    402,
    421,
    440,
    459,
    478,
    497,
    516,
    535,
    554,
    573,
    592,
    611,
    630,
    649,
    668,
    687,
    706,
    725,
    744,
    763,
    782,
    801,
    820,
    839,
    858,
    877,
    896,
    915,
    934,
    953,
    972,
    991,
    1010,
    1029,
    1048,
    1067,
    1086,
    1105,
    1124,
    1143,
    1200
};

#define TEMP_TABLE_SIZE (sizeof(temp_table) / sizeof(temp_table[0]))

static const unsigned int volt_table[]
    #ifdef KTHERM_WITH_PROGMEM
    PROGMEM
    #endif
    = {
    0,
    397,
    798,
    1203,
    1612,
    2023,
    2436,
    3225,
    4013,
    4756,
    5491,
    5694,
    6339,
    7021,
    7859,
    8619,
    9383,
    10153,
    10930,
    11712,
    12499,
    13290,
    14084,
    14881,
    15680,
    16482,
    17285,
    18091,
    18898,
    19707,
    20516,
    21326,
    22137,
    22947,
    23757,
    24565,
    25373,
    26179,
    26983,
    27784,
    28584,
    29380,
    30174,
    30964,
    31752,
    32536,
    33316,
    34093,
    34867,
    35637,
    36403,
    37166,
    37925,
    38680,
    39432,
    40180,
    40924,
    41665,
    42402,
    43134,
    43863,
    44588,
    45308,
    46024,
    46735,
    48838
};

#define VOLTAGE_TABLE_SIZE (sizeof(volt_table) / sizeof(volt_table[0]))
#define POINTS_COUNT (VOLTAGE_TABLE_SIZE - 1)

static inline unsigned long getTableVal(const unsigned int *t,
                                        unsigned char i)
{
    #ifdef KTHERM_WITH_PROGMEM
    return (unsigned long)pgm_read_word_near(t + i);
    #else
    return (unsigned long)t[i];
    #endif
}

static inline unsigned long interpolate(unsigned long val,
                                        unsigned long rangeStart,
                                        unsigned long rangeEnd,
                                        unsigned long valStart,
                                        unsigned long valEnd)
{
    unsigned long valDiff = valEnd - valStart;

    if (valDiff == 0)
        return 0;

    return rangeStart + (rangeEnd - rangeStart) * (val - valStart) / valDiff;
}

static inline unsigned long interpolateVoltage(unsigned long temp,
                                               unsigned char i)
{
    return interpolate(temp,
                       getTableVal(volt_table, i - 1),
                       getTableVal(volt_table, i),
                       getTableVal(temp_table, i - 1) * TEMP_TABLE_SCALE,
                       getTableVal(temp_table, i) * TEMP_TABLE_SCALE);
}

static inline unsigned long interpolateTemperature(unsigned long microvolts,
                                                   unsigned char i)
{
    return interpolate(microvolts,
                       getTableVal(temp_table, i - 1) * TEMP_TABLE_SCALE,
                       getTableVal(temp_table, i) * TEMP_TABLE_SCALE,
                       getTableVal(volt_table, i - 1),
                       getTableVal(volt_table, i));
}

static inline unsigned char binarySearch(const unsigned int *t,
                                         unsigned long val,
                                         unsigned long scale)
{
    unsigned char first;
    unsigned char middle;
    unsigned char last;
    unsigned long cur;

    // Note that we skip the first item in the list.
    first = 1;
    last = POINTS_COUNT - 2;
    middle = (first + last) / 2;

    while (first <= last)
    {
        cur = getTableVal(t, middle) * scale;

        if (val > cur)
        {
            first = middle + 1;
        }
        else if (val == cur)
        {
            return middle + 1;
        }
        else
        {
            last = middle - 1;
        }

        middle = (first + last) / 2;
    }

    return last + 1;
}

static inline unsigned char linearSearch(const unsigned int *t,
                                         unsigned long val,
                                         unsigned long scale)
{
    unsigned char i;

    for (i = 0; i < POINTS_COUNT; i++)
    {
        if ((getTableVal(t, i) * scale) > val)
        {
            return i;
        }
    }
    return POINTS_COUNT - 1;
}

/**
 * Returns the index of the first point whose temperature
 * value is greater than argument
 **/
static inline unsigned char searchTemp(unsigned long temp)
{
    #ifdef KTHERM_WITH_BINARY_SEARCH
    return binarySearch(temp_table, temp, TEMP_TABLE_SCALE);
    #else
    return linearSearch(temp_table, temp, TEMP_TABLE_SCALE);
    #endif
}

/**
 * Returns the index of the first point whose microvolts
 * value is greater than argument
 **/
static inline unsigned char searchMicrovolts(unsigned long microvolts)
{
    #ifdef KTHERM_WITH_BINARY_SEARCH
    return binarySearch(volt_table, microvolts, 1);
    #else
    return linearSearch(volt_table, microvolts, 1);
    #endif
}

/**
 * Returns temperature as a function of the ambient temperature
 * and measured thermocouple voltage.
 * Currently only positive ambient temperature is supported
 **/
long thermocoupleConvertWithCJCompensation(unsigned long microvoltsMeasured,
                                           unsigned long ambient)
{
    // Convert ambient temp to microvolts
    // and add them to the thermocouple measured microvolts 
    unsigned long microvolts = 
        microvoltsMeasured + interpolateVoltage(ambient, searchTemp(ambient));

    // look up microvolts in The Table and interpolate
    return interpolateTemperature(microvolts, searchMicrovolts(microvolts));
}

/**
 * Returns temperature, equivalent to the voltage provided in microvolts
 */
long thermocoupleMvToC(unsigned long microvolts)
{
    return interpolateTemperature(microvolts, searchMicrovolts(microvolts));
}

#ifdef KTHERM_TEST
int main(int argc, char **argv)
{
    unsigned long i = 0;

    if (argc < 2)
    {
        goto usage;
    }

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--temps"))
        {
            for (i = 0; i < 16383; i++)
            {
                unsigned long voltage = 5000000 / 16384 * i / 101;
                printf("%ld\n", thermocoupleMvToC(voltage));
            }
        }
        else if (!strcmp(argv[i], "--microvolts"))
        {
            for (i = 0; i < 1280000; i+= 1000)
            {
                printf("%ld\n", interpolateVoltage(i, searchTemp(i)));
            }
        }
        else
        {
            fprintf(stderr, "Unknown option %s\n", argv[i]);
            goto usage;
        }
    }

    return 0;
usage:
    printf("K-Type Thermocouple Library Tests\n");
    printf(" Usage: %s [--microvolts] [--temps]\n", argv[0]);
    return -1;
}
#endif
