#ifndef ENCODER_H
#define ENCODER_H

#include<iostream>
#include <pigpio.h>
#include <unistd.h>

#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;


class Encoder {        // The class
	
	private:
		void clockup();
		
		void clockdown();
		
		void MSB();
    
	public:          // Access specifier
		int clkPin;  // Attribute
		int datPin;  // Attribute
		int csPin;      // Attribute
		
		int delay;
		int ns;
		
		int bitcount;
		 
		Encoder();
		
		uint readpos();
};

#endif
