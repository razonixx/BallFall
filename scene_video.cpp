#include "physics_video.hpp"

int edgeThresh = 50;

cv::Mat gray, canny_output;
std::vector<std::vector<cv::Point>> contours;
std::vector<cv::Vec4i> hierarchy;

cv::VideoCapture cap;
cv::Mat frame, oldFrame;
cv::Mat grayFrame;

const char* window_name1 = "Fall Ball";

bool exitLoop = false;

void tick(Ball* ball)
{
    ball->tick();
}

void calculateContours(cv::Mat frame)
{
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    Canny(grayFrame, canny_output, edgeThresh, edgeThresh*3, 3);
    findContours( canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

    cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 2);
}

int main(int argc, const char** argv)
{
    srand(time(NULL));
    bool useVideo = false;

    if(useVideo)
    {
        cap.open("video.mp4");
    }
    else
    {
        int deviceID = 0;             // 0 = open default camera
        int apiID = cv::CAP_ANY;      // 0 = autodetect default API
        cap.open(deviceID, apiID);
    }
    cv::namedWindow(window_name1, cv::WindowFlags::WINDOW_AUTOSIZE);
    if (!cap.read(frame)) // if not success, break loop
    {
        std::cout << "Cannot read the video file" << std::endl;
    }

    Ball* ballObject = new Ball(frame, cv::Point(300,450), cv::Point2f(static_cast<float>(rand()%10-5), 0), 10, cv::Scalar(255, 0, 255), 2);
    //Ball* ballObject = new Ball(frame, cv::Point(50,450), cv::Point2f(-8, 0), 10, cv::Scalar(255, 0, 255), 2);

    while(!exitLoop)
    {
        oldFrame = frame.clone();
        if (!cap.read(frame)) // if not success, break loop
        {
            std::cout << "Cannot read the video file" << std::endl;
            break;
        }
        cv::flip(frame, frame, 1);

        if(!std::equal(oldFrame.begin<uchar>(), oldFrame.end<uchar>(), frame.begin<uchar>()))
        {
            //std::cout << "Changed frame" << std::endl;
            contours.clear();
            calculateContours(frame);
            ballObject->updateCanvas(frame, contours);
        }

        char k = cv::waitKey(1);
        switch (k)
        {
            case 'a':
                break;

            case 27: // esc
                exitLoop = true;
                cap.release();
                cv::destroyAllWindows();
                break;
            
            case 'd':
                ballObject->initialVelocity = cv::Point2f(static_cast<float>(rand()%10-5), 0);
                ballObject->debug();
                break;

            case 'f':
                break;
            
            default:
                break;
        }
        
        tick(ballObject);
        imshow(window_name1, frame);
    }
    return 0;
}
