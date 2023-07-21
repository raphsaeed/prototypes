//> g++ -std=c++11 -I /path/to/boost -I /path/to/opencv -L /path/to/boost/libs -L /path/to/opencv/libs your_file.cpp -lboost_filesystem -lboost_system -lopencv_core -lopencv_imgcodecs -lopencv_calib3d -o output_file

#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

std::vector<std::vector<cv::Point2f>> get_image_points(const std::string& directory, const cv::Size& boardSize, const cv::Size& imageSize)
{
    std::vector<std::vector<cv::Point2f>> imagePoints;
    boost::filesystem::path p(directory);
    for(auto& entry : boost::filesystem::directory_iterator(p))
    {
        cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(image, boardSize, corners);

        if(found)
        {
            cv::cornerSubPix(image, corners, cv::Size(11,11), cv::Size(-1,-1),
                             cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::MAX_ITER, 30, 0.1));
            imagePoints.push_back(corners);
        }
    }
    return imagePoints;
}

int main(int argc, char** argv)
{
    // Prepare object points
    int boardWidth = 9, boardHeight = 6;
    cv::Size boardSize = cv::Size(boardWidth, boardHeight);
    std::vector<cv::Point3f> objectPoints;
    for(int i = 0; i < boardHeight; i++)
    {
        for(int j = 0; j < boardWidth; j++)
        {
            objectPoints.push_back(cv::Point3f(j, i, 0));
        }
    }

    // Prepare image points for both cameras
    cv::Size imageSize = cv::imread(argv[1], cv::IMREAD_GRAYSCALE).size();
    std::vector<std::vector<cv::Point2f>> imagePoints1 = get_image_points(argv[1], boardSize, imageSize);
    std::vector<std::vector<cv::Point2f>> imagePoints2 = get_image_points(argv[2], boardSize, imageSize);

    // Calibrate both cameras
    cv::Mat cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2;
    std::vector<cv::Mat> rvecs1, tvecs1, rvecs2, tvecs2;
    cv::calibrateCamera(std::vector<std::vector<cv::Point3f>>(imagePoints1.size(), objectPoints),
                        imagePoints1, imageSize, cameraMatrix1, distCoeffs1, rvecs1, tvecs1);
    cv::calibrateCamera(std::vector<std::vector<cv::Point3f>>(imagePoints2.size(), objectPoints),
                        imagePoints2, imageSize, cameraMatrix2, distCoeffs2, rvecs2, tvecs2);

    // Print camera parameters for both cameras
    std::cout << "Camera Matrix 1: \n" << cameraMatrix1 << std::endl;
    std::cout << "Distortion Coefficients 1: \n" << distCoeffs1 << std::endl;
    std::cout << "Camera Matrix 2: \n" << cameraMatrix2 << std::endl;
    std::cout << "Distortion Coefficients 2: \n" << distCoeffs2 << std::endl;

    return 0;
}
