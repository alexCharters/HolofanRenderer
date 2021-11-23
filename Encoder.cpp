/*
 * Encoder.cpp
 * 
 * 
 */
#include "Encoder.h"
#include <wiringPi.h>

Encoder::Encoder() { // Constructor with parameters
  clkPin = 2;
  datPin = 3;
  csPin = 4;
  
  delay = 5;
  ns = 1;
  bitcount = 16;
  
  wiringPiSetupGpio();
  
  pinMode(clkPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(datPin, INPUT);
  pinMode(csPin, OUTPUT);
  
  digitalWrite(csPin, 1);
  digitalWrite(clkPin, 1);
  
  std::cout<<"GPIO configuration enabled\n";
}

uint Encoder::readpos(){
  digitalWrite(csPin, 0);
  sleep_for(nanoseconds(delay));
  MSB();
  uint data = 0;
  
  int i, j;
  for(i = 0; i<bitcount; i++){
    if(i < 10){
      clockup();
      for(j = 0; j < ns; j++){
        data <<= 1;
        data |= digitalRead(datPin);
      }
      clockdown();
    }
    else{
      for(j = 0; j < 6; j++){
        clockup();
        clockdown();
      }
    }
  }
  
  digitalWrite(csPin, 1);
  return data;
}
  
void Encoder::clockup(){
  digitalWrite(clkPin, 1);
}

void Encoder::clockdown(){
  digitalWrite(clkPin, 0);
}

void Encoder::MSB(){
  clockdown();
}

