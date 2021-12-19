// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string>
#include <climits>
#include <boost/filesystem.hpp>
// 3rd party header for writing png files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"

#define NUM_SLEEPS 100
using namespace boost::filesystem;
using namespace std;

void save_frame_photo(const char* filename, rs2::frameset frameset){
    rs2::colorizer color_map;
    rs2::frame frame = frameset.get_color_frame();
    if (auto image = frame.as<rs2::video_frame>())
    {
        // Write images to disk
        std::stringstream png_file;
        png_file << "rs-save-to-disk-output-" << image.get_profile().stream_name() << ".png";

        if (image.is<rs2::depth_frame>()) image = color_map.process(frame);
        stbi_write_png(filename, image.get_width(), image.get_height(),
               image.get_bytes_per_pixel(), image.get_data(), image.get_stride_in_bytes());
        std::cout << "Saved " << png_file.str() << std::endl;

    }
    
}

void save_frame_depth_data(const std::string& filename,
                           rs2::frame frame,
                           float depth_units)
{
    if (auto image = frame.as<rs2::video_frame>())
    {
        std::ofstream myfile;
        std::stringstream fullname;
        fullname << filename << "_" << frame.get_frame_number() << ".csv";
        myfile.open(fullname.str());
        myfile << std::setprecision(2);
        auto pixels = (uint16_t*)image.get_data();
        
        for (auto y = 0; y < image.get_height(); y++)
        {
            for (auto x = 0; x < image.get_width(); x++)
            {
                myfile << pixels[image.get_width() * y + x] * depth_units << ", ";
            }
            myfile << "\n";
        }
        
        myfile.close();
    }
}

void save_frame_color_data(const std::string& filename,
                           rs2::frame frame)
{
    if (auto image = frame.as<rs2::video_frame>())
    {
        std::ofstream myfile;
        std::stringstream fullname;
        fullname << filename << "_" << frame.get_frame_number() << ".csv";
        myfile.open(fullname.str());
        myfile << std::hex << std::setw(2) << std::setfill('0');
        auto pixels = (uint8_t*)image.get_data();
        
        for (auto y = 0; y < image.get_height(); y++)
        {
            for (auto x = 0; x < image.get_width(); x++)
            {
                myfile << "0x";
                for (auto c = 0; c < 3; c++)
                {
                    myfile << (int)pixels[3*(image.get_width() * y + x) + c];
                }
                myfile << ",";
            }
            myfile << "\n";
        }
        
        myfile.close();
    }
}

// Capture Example demonstrates how to
// capture depth and color video streams and render them to the screen
int main(int argc, char * argv[]) try
{

    // find the last number
    path p ("./pictures/purplering/png/");
    directory_iterator end;
    int maxnum = INT_MIN;
    for (directory_iterator itr(p); itr != end; ++itr){
        const char* name = itr->path().c_str();
        int num = atoi(name);
        maxnum = max(maxnum, num);
    }
    if (maxnum == INT_MIN) maxnum = -1; // no files in the directories yet
    maxnum++; // we want to use one more than the max num so no duplicates

    rs2::colorizer color_map;
    
    auto depth_units = rs2::context().query_devices().front()
        .query_sensors().front().get_option(RS2_OPTION_DEPTH_UNITS);
    
    rs2::pipeline pipe;
    pipe.start();

    int i;
    // focus the camera for a second
    for (i = 0; i < 30; ++i) pipe.wait_for_frames();
    i = 0;
    while(true) // Application still alive?
    {
        cout << "Iteration: " << i % NUM_SLEEPS << endl;
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera

        rs2::frame depth = data.get_depth_frame(); // Find and colorize the depth data
        rs2::frame color = data.get_color_frame();            // Find the color data

        char buff[1000];
        sprintf(buff, "./pictures/purplering/png/%d", i/NUM_SLEEPS + maxnum);
        if (i%NUM_SLEEPS == 0){
            save_frame_depth_data("./pictures/purplering/depth/" + to_string(i/NUM_SLEEPS + maxnum), depth, depth_units);
            save_frame_color_data("./pictures/purplering/color/" + to_string(i/NUM_SLEEPS + maxnum), color);
            save_frame_photo(buff, data);
        }
        i++;
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}



