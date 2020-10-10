#include "physics_video.hpp"

int edgeThresh = 70;

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
    Canny(frame, canny_output, edgeThresh, edgeThresh*3, 3);
    findContours( canny_output, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

    cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 2);
}

int main(int argc, const char** argv)
{
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

    Ball* ballObject = new Ball(frame, cv::Point(300,50), 10, cv::Scalar(255, 0, 255), 1, 1);

    ballObject->v0X = 3;
    ballObject->v0Y = 0;

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
