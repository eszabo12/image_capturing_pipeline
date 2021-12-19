
all: my_capture.cpp 
	g++ -o a my_capture.cpp -lboost_system -lboost_filesystem -lrealsense2
