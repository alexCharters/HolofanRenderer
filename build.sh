
g++ -g -Wall -Wextra -I/usr/include/ -I/usr/local/include main.cpp Encoder.cpp Fan.cpp -L/usr/local/lib -L/usr/lib -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lglut -lGLEW -lwiringPi -o main
