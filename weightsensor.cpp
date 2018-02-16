// weightsensor.cpp

//#include <Arduino.h>
#include "HX711.h"

#include <roboticscape.h>
#include <rc_usefulincludes.h>

//#if ARDUINO_VERSION <= 106
    // "yield" is not implemented as noop in older Arduino Core releases, so let's define it.
    // See also: https://stackoverflow.com/questions/34497758/what-is-the-secret-of-the-arduino-yieldfunction/34498165#34498165
    //void yield(void) {};
//#endif

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin) {
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; ++i) {
        rc_gpio_set_value_mmap(clockPin, HIGH);
        //if (bitOrder == LSBFIRST)
        //  value |= digitalRead(dataPin) << i;
        value |= rc_gpio_get_value_mmap(dataPin) << (7 - i);
        rc_gpio_set_value_mmap(clockPin, LOW);
    }
    return value;
}

HX711::HX711(uint8_t dout, uint8_t pd_sck, uint8_t gain)
        : PD_SCK(pd_sck), DOUT(dout) {
    
    rc_gpio_set_dir(PD_SCK, OUTPUT_PIN);
    rc_gpio_set_dir(DOUT, INPUT_PIN);

    set_gain(gain);
}

/*
HX711::HX711() {
}

HX711::~HX711() {
}
*/

//void HX711::begin(uint8_t dout, uint8_t pd_sck, uint8_t gain) {
//  PD_SCK = pd_sck;
//  DOUT = dout;
//
//}

bool HX711::is_ready() {
    return rc_gpio_get_value_mmap(DOUT) == LOW;
}

void HX711::set_gain(uint8_t gain) {
    switch (gain) {
        case 128:       // channel A, gain factor 128
            GAIN = 1;
            break;
        case 64:        // channel A, gain factor 64
            GAIN = 3;
            break;
        case 32:        // channel B, gain factor 32
            GAIN = 2;
            break;
    }

    rc_gpio_set_value_mmap(PD_SCK, LOW);
}



long HX711::read() {
    // wait for the chip to become ready
    if (is_ready()) {
    

        unsigned long value = 0;
        uint8_t data[3] = { 0 };
        uint8_t filler = 0x00;

        // pulse the clock pin 24 times to read the data
        data[2] = shiftIn(DOUT, PD_SCK);
        data[1] = shiftIn(DOUT, PD_SCK);
        data[0] = shiftIn(DOUT, PD_SCK);

        // set the channel and the gain factor for the next reading using the clock pin
        for (unsigned int i = 0; i < GAIN; i++) {
            rc_gpio_set_value_mmap(PD_SCK, HIGH);
            rc_gpio_set_value_mmap(PD_SCK, LOW);
        }

        // Replicate the most significant bit to pad out a 32-bit signed integer
        if (data[2] & 0x80) {
            filler = 0xFF;
        } else {
            filler = 0x00;
        }

        // Construct a 32-bit signed integer
        value = ( static_cast<unsigned long>(filler) << 24
                | static_cast<unsigned long>(data[2]) << 16
                | static_cast<unsigned long>(data[1]) << 8
                | static_cast<unsigned long>(data[0]) );

        return static_cast<long>(value);
    }

    return 0;
}

long HX711::read_average(uint8_t times) {
    long sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += read();
        //yield();
    }
    return sum / times;
}

double HX711::get_value(uint8_t times) {
    return read_average(times) - OFFSET;
}

float HX711::get_units(uint8_t times) {
    return get_value(times) / SCALE;
}

void HX711::tare(uint8_t times) {
    double sum = read_average(times);
    set_offset(sum);
}

void HX711::set_scale(float scale) {
    SCALE = scale;
}

float HX711::get_scale() {
    return SCALE;
}

void HX711::set_offset(long offset) {
    OFFSET = offset;
}

long HX711::get_offset() {
    return OFFSET;
}

void HX711::power_down() {
    rc_gpio_set_value_mmap(PD_SCK, LOW);
    rc_gpio_set_value_mmap(PD_SCK, HIGH);
}

void HX711::power_up() {
    rc_gpio_set_value_mmap(PD_SCK, LOW);
}

