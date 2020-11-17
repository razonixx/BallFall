#include "physics_video.hpp"

int edgeThresh = 50;
int contourThickness = 1;

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

    if(contourThickness > 0)
    {
        cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), contourThickness);
    }
}

int main(int argc, const char** argv)
{
    srand(time(NULL));
    bool useVideo = false;
    int frameDelay = 15;

    if(argc == 2)
    {
        cap.open(argv[1]);
        frameDelay = 16;
        edgeThresh = 100;
        useVideo = true;
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

    Ball* ballObject = new Ball(frame, cv::Point(30,45), cv::Point2f(3, 0), 10, cv::Scalar(255, 0, 255), 1.3);

    while(!exitLoop)
    {
        oldFrame = frame.clone();
        if (!cap.read(frame)) // if not success, break loop
        {
            std::cout << "Video file empty" << std::endl;
            exitLoop = true;
            break;
        }
        if(!useVideo)
        {
            cv::flip(frame, frame, 1);
        }

        contours.clear();
        calculateContours(frame);
        ballObject->updateCanvas(frame, contours, edgeThresh);

        char k = cv::waitKey(frameDelay);
        switch (k)
        {
            case 27: // esc
                exitLoop = true;
                cap.release();
                cv::destroyAllWindows();
                break;
            
            case 'd':
                ballObject->debug();
                break;

            case 82:
                edgeThresh -= 10;
                break;

            case 84:
                edgeThresh += 10;
                break;

            case 81:
                contourThickness--;
                break;

            case 83:
                contourThickness++;
                break;
            
            default:
                break;
        }
        
        tick(ballObject);
        imshow(window_name1, frame);
    }
    return 0;
}
