/*
 * Fan.cpp
 * 
 */

#include "Fan.h"

Fan::Fan(){
	wiringPiSPISetup(0, 32000000) ;
	srand(time(NULL));
	
	
	int i, j;
	for(i = 0; i < radialResolution; i++){
		bufferOne[i*(4*pixels+8)] = 0;
		bufferOne[i*(4*pixels+8)+1] = 0;
		bufferOne[i*(4*pixels+8)+2] = 0;
		bufferOne[i*(4*pixels+8)+3] = 0;
		bufferOne[(i+1)*(4*pixels+8)-1] = 0xFF;
		bufferOne[(i+1)*(4*pixels+8)-2] = 0xFF;
		bufferOne[(i+1)*(4*pixels+8)-3] = 0xFF;
		bufferOne[(i+1)*(4*pixels+8)-4] = 0xFF;
		
		bufferTwo[i*(4*pixels+8)] = 0;
		bufferTwo[i*(4*pixels+8)+1] = 0;
		bufferTwo[i*(4*pixels+8)+2] = 0;
		bufferTwo[i*(4*pixels+8)+3] = 0;
		bufferTwo[(i+1)*(4*pixels+8)-1] = 0xFF;
		bufferTwo[(i+1)*(4*pixels+8)-2] = 0xFF;
		bufferTwo[(i+1)*(4*pixels+8)-3] = 0xFF;
		bufferTwo[(i+1)*(4*pixels+8)-4] = 0xFF;
		
		for(j = 0; j < pixels; j++){
			bufferOne[i*(4*pixels+8)+4+j*4] = 0xE1;
			bufferTwo[i*(4*pixels+8)+4+j*4] = 0xE1;
		}
	}
	
	std::thread render_thread(&Fan::render, this);
	render_thread.detach();
}

void Fan::render(){
	while(true){
		int line = rand() % 512;
		if(usingBufferOne){
			wiringPiSPIDataRW(0, &bufferTwo[line*(128*4 + 8)], 128*3);
		}
		else{
			wiringPiSPIDataRW(0, &bufferOne[line*(128*4 + 8)], 128*3);
		}
	}
}

void Fan::writeFrame(unsigned char* frame){
	uint8_t* buffToWrite;
	if(usingBufferOne){
		buffToWrite = bufferOne;
		std::cout << "buff one\n";
	}
	else{
		buffToWrite = bufferTwo;
		std::cout << "buff two\n";
	}
	
	int i, j;
	for(i = 0; i < radialResolution; i++){
		int offset = i*(4*pixels+8)+4;
		int dataOffset = i*(128*3);
		for(j = 0; j < pixels; j++){
			buffToWrite[offset+j*4+1] = frame[dataOffset+j*3];
			buffToWrite[offset+j*4+2] = frame[dataOffset+j*3+1];
			buffToWrite[offset+j*4+3] = frame[dataOffset+j*3+2];
		}
	}
	
	usingBufferOne = !usingBufferOne;
}

void Fan::printBuffer(uint8_t* buff){
	int i, j;
	for(i = 0; i < radialResolution; i++){
		int k;
		for(k = 0; k < 4; k++){
			std::cout << (int)buff[i*(4*128+8)+k] << " ";
		}
		
		for(j = 0; j < pixels; j++){
			std::cout << "| ";
			for(k = 0; k < 4; k++){
				std::cout << (int)buff[i*(4*128+8)+4+j*4+k] << " ";
			}
		}
		std::cout << "| ";;;;;
		for(k = 0; k < 4; k++){
			std::cout << (int)buff[(i+1)*(4*128+8)-(k+1)] << " ";
		}
		std::cout << "\n";
	}
}

//int main(){
	//Fan fan;
//}


