CC=g++
# FLAGS=-Wc++11-extensions
FLAGS = -std=c++11

all: my_capture.cpp 
	$(CC) $(FLAGS) -o  a my_capture.cpp -lboost_system -lboost_filesystem -lrealsense2
