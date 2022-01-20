#include <iostream>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <pigpio.h>
#include <Encoder.h>
#include <csignal>
#include <cmath>

#ifndef FAN_H
#define FAN_H
class Fan{
	private:
		int pi;
		int spi;
	
		bool usingBufferOne = true;
		
		static const int radialResolution = 1024;
		static const int pixels = 128;
		
		void render();
		
		Encoder encoder;
	
	
	public:
		int radialOffset = 290;
		char bufferOne[(4*128+8)*radialResolution];
		char bufferTwo[(4*128+8)*radialResolution];
	
		Fan();
		
		
		void printBuffer(char* buff);
		void switchBuffers();
		void writeFrame(char* frame);
		
		int lastLine;
		
};
#endif
