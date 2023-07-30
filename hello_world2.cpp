//> g++ -o hello_world3 hello_world3.cpp `pkg-config --cflags --libs opencv4` -lpthread
#include </usr/include/opencv4/opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char** argv)
{
    // Create Gstreamer pipelines for two video sources
    cv::VideoCapture cap1("v4l2src device=/dev/video0 ! videoconvert ! appsink", cv::CAP_GSTREAMER);
    cv::VideoCapture cap2("v4l2src device=/dev/video2 ! videoconvert ! appsink", cv::CAP_GSTREAMER);

    if(!cap1.isOpened() || !cap2.isOpened())
    {
        std::cout<<"VideoCapture not opened"<<std::endl;
        exit(-1);
    }

    cv::Mat frame1, frame2;
    int imageCounter1 = 0, imageCounter2 = 0;
    int maxImages = 10; // number of images you want to capture

    std::string outputDirectory = "/media/raphs/RaphsORIN/projects/GitHUB/prototypes/images/"; // specify the output directory here

    // Create windows to display the videos
    cv::namedWindow("Live Video Stream 1", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Live Video Stream 2", cv::WINDOW_AUTOSIZE);

    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();
    auto lastImageTime = start;

    while(imageCounter1 < maxImages && imageCounter2 < maxImages)
    {
        if (!cap1.read(frame1) || !cap2.read(frame2)) 
        {
            std::cout<<"Capture read error"<<std::endl;
            break;
        }

        // Display the videos
        cv::imshow("Live Video Stream 1", frame1);
        cv::imshow("Live Video Stream 2", frame2);
        
        // Calculate the elapsed time
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsedSinceLastImage = std::chrono::duration_cast<std::chrono::seconds>(now - lastImageTime).count();

        // Save frames every 3 seconds
        if(elapsedSinceLastImage >= 3) 
        {
            cv::imwrite(outputDirectory + "frame1_" + std::to_string(imageCounter1) + ".jpg", frame1);
            imageCounter1++;

            cv::imwrite(outputDirectory + "frame2_" + std::to_string(imageCounter2) + ".jpg", frame2);
            imageCounter2++;

            lastImageTime = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // sleep for ~33ms (30fps)

        // Break the loop if the user presses the 'q' key
        if(cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Wait for 5 seconds before closing
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Destroy the windows after exiting the loop
    cv::destroyWindow("Live Video Stream 1");
    cv::destroyWindow("Live Video Stream 2");

    return 0;
}

