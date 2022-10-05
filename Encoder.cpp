/*
 * Encoder.cpp
 * 
 * 
 */
#include "Encoder.h"



Encoder::Encoder() { // Constructor with parameters
  clkPin = 17;
  datPin = 27;
  csPin = 4;
  
  delay = 1;
  ns = 1;
  bitcount = 16;
  
  if (gpioInitialise() < 0){
    std::cout<<"encoder pi pigpio err\n";
    return;
  };
  
  gpioSetMode(clkPin, PI_OUTPUT);
  gpioSetMode(datPin, PI_INPUT);
  gpioSetMode(csPin, PI_OUTPUT);
  
  gpioWrite(csPin, 1);
  gpioWrite(clkPin, 1);
  
  usleep(500000);
  
  std::cout<<"GPIO configuration enabled\n";
}

void Encoder::clockup(){
  gpioWrite(clkPin, 1);
}

void Encoder::clockdown(){
  gpioWrite(clkPin, 0);
}

void Encoder::MSB(){
  clockdown();
}

uint Encoder::readpos(){
  gpioWrite(csPin, 0);
  usleep(20);
  MSB();
  uint data = 0;
  
  int i, j;
  for(i = 0; i<bitcount; i++){
    if(i<10){
      clockup();
      gpioWrite(clkPin, 1);
      for(j = 0; j < ns; j++){
        data <<= 1;
        usleep(20);
        data |= gpioRead(datPin);
      }
      clockdown();
      gpioWrite(clkPin, 0);
    }
    else{
      clockup();
      gpioWrite(clkPin, 1);
      for(j = 0; j < ns; j++){
        data <<= 1;
        usleep(20);
        data |= gpioRead(datPin);
      }
      clockdown();
      gpioWrite(clkPin, 0);
    }
  }
  
  //std::cout<<flags<<"\n";
  
  gpioWrite(csPin, 1);

  return data;
}

