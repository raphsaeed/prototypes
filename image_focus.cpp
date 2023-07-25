//> g++ -o image_focus image_focus.cpp $(pkg-config --libs --cflags opencv4)
#include </usr/include/opencv4/opencv2/opencv.hpp>
#include <iostream>

double variance_of_laplacian(cv::Mat image) {
    cv::Mat lap;
    cv::Laplacian(image, lap, CV_64F);

    cv::Scalar mu, sigma;
    cv::meanStdDev(lap, mu, sigma);

    return sigma.val[0] * sigma.val[0];
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: ./focus_check <image_path>\n";
        return -1;
    }

    cv::Mat src = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    if (src.empty()) {
        std::cout << "Could not open or find the image\n";
        return -1;
    }

    double fm = variance_of_laplacian(src);

    std::cout << "Focus measure: " << fm << "\n";
    if (fm < 100.0) { // Threshold is hypothetical and should be determined based on your specific requirements.
        std::cout << "The image is blurry.\n";
    } else {
        std::cout << "The image is in focus.\n";
    }

    return 0;
}

