//> g++ -o focus_check focus_check.cpp $(pkg-config --libs --cflags opencv4)
#include <opencv2/opencv.hpp>
#include <iostream>

double variance_of_laplacian(const cv2::Mat& src) {
    cv2::Mat lap;
    cv2::Laplacian(src, lap, CV_64F);

    cv2::Scalar mu, sigma;
    cv2::meanStdDev(lap, mu, sigma);

    return sigma.val[0] * sigma.val[0];
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: ./focus_check <image_path>\n";
        return -1;
    }

    cv2::Mat src = cv2::imread(argv[1], cv2::IMREAD_GRAYSCALE);
    if (src.empty()) {
        std::cout << "Could not open or find the image\n";
        return -1;
    }

    double fm = variance_of_laplacian(src);

    std::cout << "Focus measure: " << fm << std::endl;
    if (fm < 100.0) { // Threshold is hypothetical and should be determined based on your specific requirements.
        std::cout << "The image is blurry.\n";
    } else {
        std::cout << "The image is in focus.\n";
    }

    return 0;
}
