//HX711.cpp

extern "C" {
#include <roboticscape.h>
#include <rc_usefulincludes.h>
}

#include <stdio.h>
#include <stdint.h>

#include "HX711.h"

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

HX711::HX711(uint8_t dout, uint8_t pd_sck, HX711_MODE mode)
  : pd_sck_(pd_sck), dout_(dout) {
    
  rc_gpio_set_dir(pd_sck_, OUTPUT_PIN);
  rc_gpio_set_dir(dout_, INPUT_PIN);

  set_mode(mode));
}

bool HX711::is_ready() {
  return rc_gpio_get_value_mmap(dout_) == LOW;
}

void HX711::set_mode(HX711_MODE mode) {
  mode_ = mode;
  read(); //mode change only occurs after a read
}


long HX711::read() {
  if(mode_ != HX711::NONE){

    // wait for the chip to become ready
    if (is_ready()) {
    

      unsigned long value = 0;
      uint8_t data[3] = { 0 };
      uint8_t filler = 0x00;

      // pulse the clock pin 24 times to read the data
      data[2] = shiftIn(dout_, pd_sck_);
      data[1] = shiftIn(dout_, pd_sck_);
      data[0] = shiftIn(dout_, pd_sck_);

      // set the mode for the next reading using the clock pin
      for (int i = 0; i < (int)mode_; i++) {
        rc_gpio_set_value_mmap(pd_sck_, HIGH);
        rc_gpio_set_value_mmap(pd_sck_, LOW);
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
  return read_average(times) - offset_;
}

float HX711::get_units(uint8_t times) {
  return get_value(times) / scale_;
}

void HX711::tare(uint8_t times) {
  double sum = read_average(times);
  set_offset(sum);
}

void HX711::set_scale(float scale) {
  scale_ = scale;
}

float HX711::get_scale() {
  return scale_;
}

uint8_t HX711::get_gain(){
  return gains_[(int)mode_];
}

void HX711::set_offset(long offset) {
  offset_ = offset;
}

long HX711::get_offset() {
  return offset_;
}

void HX711::power_down() {
  rc_gpio_set_value_mmap(pd_sck_, LOW);
  rc_gpio_set_value_mmap(pd_sck_, HIGH);
}

void HX711::power_up() {
  rc_gpio_set_value_mmap(pd_sck_, LOW);
}


const int dout_ = 0, pd_sck_ = 0;


