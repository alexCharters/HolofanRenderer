g++ -g -Wall -Wextra -pthread -I/usr/include/ -I/usr/local/include main_i2c.cpp Fan.cpp Encoder.cpp -L/usr/local/lib -L/usr/lib -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lglut -lGLEW -lwiringPi -lpigpio -lrt -lpigpiod_if2 -o main_i2c
