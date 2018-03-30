#include <unistd.h>
#include <iostream>

extern "C" {
#include <roboticscape.h>
#include <rc_usefulincludes.h>
}



//variables and constants

int slaveSelect = 1; //the slave select line, let it be 1 
char dataReadyTX = 0x00; //byte of all 0 to write to MOSI in order to query the state of DOUT on MISO
char dataReadyRX; //byte to contain DOUT state. In context, 0x00 means data is ready, 0x01 means data is not ready
char dataRX[8]; //data RX array (comes in on MISO)
char dataTX[8]= {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x80,0x00}; //data TX array (goes out on MOSI)

//helper functions

//set up blue
bool init(){
  //perform setup
  if(rc_initialize() == -1){
    //additional error logging here
    return false;
  }
  //inits SPI 1.1 with auto slave select 1, at a clock frequency of 80 kHz
  //SPI_MODE_CPOL0_CPHA1 means the clock is 0 when idle and data is read in on the falling edge
  if(rc_spi_init(SS_MODE_AUTO, SPI_MODE_CPOL0_CPHA1, 80000, slaveSelect) == -1){ 
    //additional error logging here
    return false;
  }
  return true;
}

//check if data is available (hx711 DOUT is low)
bool isReady(){
  if(rc_spi_transfer(&dataReadyTX, 1, &dataReadyRX, slaveSelect) == -1){ //write 0x00 to MOSI and query the status of MISO
    //additional error logging here
    throw 0;
  }
  return !(dataReadyRX | 0XFE) ; //since 0x00 is what we want, bitmask all but the last bit to 1 and then invert
}

bool readData(){
  memset(dataRX, 0, sizeof(dataRX));

  //sets the mode to channel A gain 128
  if(rc_spi_transfer(dataTX, 8, dataRX, slaveSelect) == -1){ //read in data, each DOUT bit sampled twice
    //additional error logging here
    return -1;
  }
  
  //convert the 8 byte dataRX to the 3 actual bytes of data (24 bits) by dropping the last two bytes and sampling every other bit
  //converData(dataRX); //not implemented since i'm lazy
}

int main(void){

  std::cout<< "starting..." << std::endl;
  
  if(!init()){
    //additional error logging here
    return -1;
  }

  std::cout << "initialized..." << std::endl;
  
  while(rc_get_state() != EXITING){
    while(isReady() && rc_get_state() != EXITING){
      usleep(25000); // 25 ms
    }    
    std::cout << "Data is ready" << std::endl;
    readData();
    for(int i = 0; i < 8; i++){
      std::cout << "Data-" << i << ": " << (int) dataRX[i] << std::endl;
    }
    usleep(250000);
  }
  std::cout << "exiting..." << std::endl;

  return 1;
  
}
