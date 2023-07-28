#include </usr/include/opencv4/opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char** argv)
{
    // Create Gstreamer pipeline
    cv::VideoCapture cap("v4l2src device=/dev/video0 ! videoconvert ! appsink", cv::CAP_GSTREAMER);

    if(!cap.isOpened())
    {
        std::cout<<"VideoCapture not opened"<<std::endl;
        exit(-1);
    }

    cv::Mat frame;
    int frameCounter = 0;

    while(true)
    {
        if (!cap.read(frame)) 
        {
            std::cout<<"Capture read error"<<std::endl;
            break;
        }
        
        // Save frame every 3 seconds
        if(frameCounter % 90 == 0) // Assuming 30 frames per second, adjust accordingly
        {
            cv::imwrite("frame" + std::to_string(frameCounter/30) + ".jpg", frame);
        }

        frameCounter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // sleep for ~33ms (30fps)
    }

    return 0;
}
