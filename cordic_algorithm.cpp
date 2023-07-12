#include <iostream>
#include <cmath>

using namespace std;

// Constants for the CORDIC algorithm
const double CORDIC_K = 0.6072529350088812561694; // K = 1 / sqrt(2)
const int CORDIC_ITERATIONS = 16;

// Calculate the sine wave frequency using the CORDIC algorithm
double cordicSinWaveFrequency(double phase) {
    double x = CORDIC_K;
    double y = 0.0;
    double angle = phase;

    for (int i = 0; i < CORDIC_ITERATIONS; i++) {
        double d = (angle >= 0) ? 1 : -1;
        double newX = x - d * y * pow(2, -i);
        double newY = y + d * x * pow(2, -i);
        double newAngle = angle - d * atan(pow(2, -i));

        x = newX;
        y = newY;
        angle = newAngle;
    }

    return x;
}

int main() {
    // Set the phase angle of the sine wave in radians
    double phase = 1.0;

    // Calculate the sine wave frequency using the CORDIC algorithm
    double frequency = cordicSinWaveFrequency(phase);

    cout << "Sine wave frequency: " << frequency << endl;

    return 0;
}
