//> g++ -std=c++11 -pthread sensors.cpp -o sensors
// -pthread
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx; // Mutex to protect shared data
std::condition_variable cv; // Condition variable for synchronization

bool dataReady = false; // Shared variable

void sensor1() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate work

        // Lock and update shared data
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Sensor 1 produced data\n";
        dataReady = true;

        // Notify main thread
        cv.notify_one();
    }
}

void sensor2() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2)); // simulate work

        // Lock and update shared data
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Sensor 2 produced data\n";
        dataReady = true;

        // Notify main thread
        cv.notify_one();
    }
}

int main() {
    std::thread t1(sensor1);
    std::thread t2(sensor2);

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Wait until a sensor has produced data
        cv.wait(lock, []{ return dataReady; });

        std::cout << "Main thread consuming data\n";

        // Reset shared variable
        dataReady = false;
    }

    t1.join();
    t2.join();

    return 0;
}
