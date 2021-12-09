/*
 * Fan.cpp
 * 
 */

#include "Fan.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 
  
void signalHandler( int signum ) {
   std::cout << "Interrupt signal (" << signum << ") received. yeet.\n";

   gpioTerminate();

   exit(signum);  
}

Fan::Fan(){
	if(gpioInitialise() < 0){
		std::cout<<"pigpio err\n";
		return;
	}
	
	signal(SIGINT, signalHandler); 
	signal(SIGABRT, signalHandler); 
	signal(SIGTERM, signalHandler); 
	
	spi = spiOpen(0, int(8e6), 0xEF);
	
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
			bufferOne[i*(4*pixels+8)+4+j*4] = 0xC1;
			bufferTwo[i*(4*pixels+8)+4+j*4] = 0xC1;
		}
	}
	
	std::thread render_thread(&Fan::render, this);
	render_thread.detach();
}

void Fan::render(){
	while(true){
		int line = encoder.readpos();
		//printf("m: " BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(line>>8), BYTE_TO_BINARY(line));
		int flags = line&0x3F;
		//printf("m: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(flags));
		if(!(flags & 0x20) || flags & 0x10) continue;
		
		line = line>>6;
		//std::cout<<"line: "<<line<<"\n";
		line = (line-radialOffset)%1024;
		line=line<0?line+radialResolution:line;
		//std::cout<<"offset line: "<<line<<"\n\n";
		
		lastLine = line;
		
		//std::cout<<(line & 0x001F)<<"\n";
		//std::cout<<(!(line & 0x001F))<<"\n";
		//printf("m: " BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(line>>8), BYTE_TO_BINARY(line));
		//std::cout<<(line >> 7)<<"\n";
		
		//std::cout<<(line)<<"\n";
		//printf("m: " BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(line>>8), BYTE_TO_BINARY(line));
		
		if(usingBufferOne){
			spiWrite(spi, &bufferTwo[(line)*(128*4 + 8)], 128*4+8);
		}
		else{
			spiWrite(spi, &bufferOne[(line)*(128*4 + 8)], 128*4+8);
		}
		
	}
}

void Fan::writeFrame(char* frame){
	if(usingBufferOne){
		int i, j;
		for(i = 0; i < radialResolution; i++){
			int offset = i*(4*pixels+8)+4;
			int dataOffset = i*(128*3);
			for(j = 0; j < pixels; j++){
				//std::cout<<offset+j*4+1<<" "<<dataOffset+j*3<<"\n";
				bufferOne[offset+j*4+1] = frame[dataOffset+j*3+2]; //b
				bufferOne[offset+j*4+2] = frame[dataOffset+j*3+1]; //g
				bufferOne[offset+j*4+3] = frame[dataOffset+j*3]; //r
			}
		}
	}
	else{
		int i, j;
		for(i = 0; i < radialResolution; i++){
			int offset = i*(4*pixels+8)+4;
			int dataOffset = i*(128*3);
			for(j = 0; j < pixels; j++){
				//std::cout<<offset+j*4+1<<" "<<dataOffset+j*3<<"\n";
				bufferTwo[offset+j*4+1] = frame[dataOffset+j*3+2];
				bufferTwo[offset+j*4+2] = frame[dataOffset+j*3+1];
				bufferTwo[offset+j*4+3] = frame[dataOffset+j*3];
			}
		}
	}
	
	
	
	usingBufferOne = !usingBufferOne;
}

void Fan::printBuffer(char* buff){
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


