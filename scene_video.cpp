#include "physics_video.hpp"

int edgeThresh = 100;

cv::Mat gray, canny_output;
std::vector<std::vector<cv::Point> > contours;
std::vector<cv::Vec4i> hierarchy;
std::vector<int> convexHull;
std::vector<std::vector<int> >convexHullCollection;
std::vector<cv::Point> convexHullPoints;

cv::VideoCapture cap;
cv::Mat frame, oldFrame;
cv::Mat grayFrame;

const char* window_name1 = "Fall Ball";

bool exitLoop = false;

void tick(Ball* ball)
{
    ball->tick();
}

void calculateConvexHull(cv::Mat frame)
{
    Canny(frame, canny_output, edgeThresh, edgeThresh*3, 3);
    findContours( canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

    for (int i = 0; i < contours.size(); i++)
    {
        cv::convexHull(cv::Mat(contours[i]), convexHull, true);
        convexHullCollection.push_back(convexHull);
    }

    
    int hullcount = (int)convexHullCollection.size();
    for (int i = 0; i < contours.size(); i++)
    {
        convexHull = convexHullCollection[i];

        int hullcount = (int)convexHull.size();

        cv::Point pt0 = contours[i][convexHull[hullcount-1]];
    
        for( int j = 0; j < hullcount; j++ )
        {
            cv::Point pt = contours[i][convexHull[j]];
            convexHullPoints.push_back(pt0);
            convexHullPoints.push_back(pt);
            pt0 = pt;
        }
    }
}

void drawCollisionLines(cv::Mat frame, std::vector<cv::Point> _convexHullPoints)
{
    int pointCount = 0;
    while(pointCount < _convexHullPoints.size())
    {
        line(frame, _convexHullPoints[pointCount], _convexHullPoints[pointCount+1], cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
        pointCount+=2;
    }
}


int main(int argc, const char** argv)
{
    bool useVideo = false;
    bool useWebcam = true;

    if(useWebcam)
    {
        int deviceID = 0;             // 0 = open default camera
        int apiID = cv::CAP_ANY;      // 0 = autodetect default API
        cap.open(deviceID, apiID);
    }

    if(useVideo)
    {
        cap.open("video.mp4");
    }

    if(!useVideo && !useWebcam)
    {
        frame = cv::imread(cv::samples::findFile("Map.png"), cv::IMREAD_COLOR);
    }

    cv::namedWindow(window_name1, cv::WindowFlags::WINDOW_AUTOSIZE);

    if ((useWebcam || useVideo) && !cap.read(frame)) // if not success, break loop
    {
        std::cout << "Cannot read the video file" << std::endl;
    }

    calculateConvexHull(frame);
    //drawCollisionLines(frame, convexHullPoints);
    Ball* ballObject = new Ball(frame, cv::Point(300,50), 10, cv::Scalar(255, 0, 255), convexHullPoints, 1, 1);

    ballObject->v0X = 15;
    ballObject->v0Y = 0;

    while(!exitLoop)
    {
        oldFrame = frame.clone();
        if ((useWebcam || useVideo) && !cap.read(frame)) // if not success, break loop
        {
            std::cout << "Cannot read the video file" << std::endl;
            break;
        }
        if(!std::equal(oldFrame.begin<uchar>(), oldFrame.end<uchar>(), frame.begin<uchar>()))
        {
            //std::cout << "Changed frame" << std::endl;
            convexHullCollection.clear();
            convexHullPoints.clear();
            calculateConvexHull(frame);
            drawCollisionLines(frame, convexHullPoints);
            ballObject->updateCanvas(frame, convexHullPoints);
        }

        char k = cv::waitKey(1);
        switch (k)
        {
            case 'a':
                convexHullCollection.clear();
                convexHullPoints.clear();
                calculateConvexHull(frame);
                drawCollisionLines(frame, convexHullPoints);
                ballObject->updateCanvas(frame, convexHullPoints);
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
                drawCollisionLines(frame, convexHullPoints);
                break;
            
            default:
                break;
        }
        
        tick(ballObject);
        imshow(window_name1, frame);
    }
    return 0;
}
