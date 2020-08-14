#include <iostream> // Basic Input Output Library for C++
#include <string.h> // String Library for C / C++
#include "opencv2/core/core.hpp" // Core Opencv library
#include "opencv2/opencv.hpp" // Main Opencv library
#include "opencv2/videoio/videoio.hpp"  // Opencv library for input and output
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgproc/imgproc.hpp" //Opencv library to get image

//Compile command
//g++ camtest.cpp -o camtest -lopencv_core -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc

//Execute command
//./camtest
using namespace std;
using namespace cv;
 
int main()
{
    VideoCapture capture(0); // Initializing the camera

    if(!capture.isOpened()){ // Show error is unable to open camera
     cout << "Failed to connect to the camera." << endl;
    }
    Mat img; // Variable to store captured image data
    capture >> img; // Capture Image data
    if(img.empty()){ // Show erroe if image is empty
 cout << "Failed to capture an image" << endl;
 return -1;
    }

    imwrite("BBImage.png", img); // Save the captured image with filename
    return 0;
}