#include <iostream>
#include <stdint.h>
#include <wiringPiSPI.h>
#include <thread>
#include <mutex>
#include <stdlib.h>

#ifndef FAN_H
#define FAN_H
class Fan{
	private:
		bool usingBufferOne = true;
		
		static const int radialResolution = 512;
		static const int pixels = 128;
		
		void render();
		
	
	
	public:
		uint8_t bufferOne[(4*128+8)*radialResolution];
		uint8_t bufferTwo[(4*128+8)*radialResolution];
	
		Fan();
		
		void printBuffer(uint8_t* buff);
		void switchBuffers();
		void writeFrame(unsigned char* frame);
		
};
#endif
