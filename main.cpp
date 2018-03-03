#include "HX711.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

extern "C" {
#include<roboticscape.h>
#include<rc_usefulincludes.h>
}

uint8_t pd_sck = 49;
uint8_t dout = 57;

int main(void)
{
  //HX711 loadcells(dout_, pd_sck_, 128);
  HX711 loadcells(pd_sck, dout, HX711_MODE::A_128);
  //loadcells.set_scale(2280.0f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  //loadcells.tare();

  /*printf("\nBefore setting up the scale:");
    printf("\nread: \t\t");
    printf("%d", loadcells.read());           // print a raw reading from the ADC

    printf("\nread average: \t\t");
    printf("\n%d", loadcells.read_average(20));   // print the average of 20 readings from the ADC

    printf("get value: \t\t");
    printf("\n%d", loadcells.get_value(5));

    printf("\nAfter setting up the scale:");
    printf("\n%d", loadcells.get_units(5)); */

  while(rc_get_state()!= EXITING)
    {
      //printf("\n%d", loadcells.get_units(10)); // find the average of 10 trials
      printf("\n %d", loadcells.read()); 
      //rc_gpio_set_value_mmap(116, HIGH); //what does this do?
      sleep(1);
    }

  return -1;
} 
