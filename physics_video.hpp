#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"


#include <iostream>
#include <cmath>

class Ball
{
    private:
        cv::Mat _canvas;
        int _radius;
        cv::Scalar _color;
        float _bounce;
        float _weight;
        float _gravity;
        float _xImpulse;
        float _yImpulse;
        cv::Scalar _colorWhite = cv::Scalar(255, 255, 255);
        std::vector<cv::Point> _convexHullPoints;

        cv::Point2f _initialPosition;

    public:
        cv::Point2f position;
        float milisecondsSinceLastCollision;
        float x0 = 0, y0 = 0;
        float v0X, v0Y;

        std::vector<int> convexHull;
        

    public: 
        Ball(
            cv::Mat canvas,
            cv::Point2f _position, 
            int radius, cv::Scalar color, 
            std::vector<cv::Point> convexHullPoints,
            float bounce, 
            float weight
            )
        {
            _canvas = canvas;
            position = _position;
            _radius = radius;
            _color = color;
            _convexHullPoints = convexHullPoints;
            _bounce = bounce;
            _weight = weight;
            _gravity = 9.8;

            _initialPosition = _position;

            milisecondsSinceLastCollision = 0;
        }

        ~Ball()
        {

        }

        void tick()
        {
            cv::circle(_canvas, position, _radius, _colorWhite, cv::LineTypes::FILLED);

            if(!checkCollisionWithAllLines())
            {

                milisecondsSinceLastCollision+=.01;

                _xImpulse = x0 + (v0X * milisecondsSinceLastCollision); // x=x0+vxt
                _yImpulse = (float)(y0 + 0.5 * (v0Y + (v0Y - _gravity * milisecondsSinceLastCollision)) * milisecondsSinceLastCollision); // https://openstax.org/books/university-physics-volume-1/pages/4-3-projectile-motion

                std::cout << "xImpulse: " << _xImpulse << std::endl;
                std::cout << "yImpulse: " << _yImpulse << std::endl;
                position.x += _xImpulse;
                position.y -= _yImpulse;

                cv::circle(_canvas, position, _radius, _color, cv::LineTypes::FILLED);

                std::cout << "Angle of movement: " << getAngleOfMovement() << std::endl;
            }
            else
            {
                collide();
            }
            
        }

        bool checkCollisionWithAllLines()
        {
            int pointCount = 0;
            while(pointCount < _convexHullPoints.size())
            {
                if(pointIntersectsLine(position, _convexHullPoints[pointCount], _convexHullPoints[pointCount+1]))
                {
                    return true;
                }
                pointCount+=2;
            }
            return false;
        }

        void collide()
        {
            milisecondsSinceLastCollision = 0;
            
            std::cout << "Collision detected. Angle before colliding: " << getAngleOfMovement() << std::endl;

            position = _initialPosition;
            cv::waitKey();
        }

        float getAngleOfMovement()
        {
            return atan2(_yImpulse, _xImpulse) * (180.0 / M_PI);
        }

        void debug()
        {
            //_yImpulse = -100;
            //std::cout << "X impulse: " << _xImpulse << std::endl << "Y impulse: " << _yImpulse << std::endl;
            //std::cout << "Movement angle:" << getAngleOfMovement() << std::endl;

            checkCollisionWithAllLines();

        }

        bool pointIntersectsLine(cv::Point2f ball, cv::Point p1, cv::Point p2)
        {
            return floor(distanceBetweenTwoPoints(p1, ball)) + floor(distanceBetweenTwoPoints(p2, ball)) == floor(distanceBetweenTwoPoints(p1, p2));
        }

        float distanceBetweenTwoPoints(cv::Point p1, cv::Point p2)
        {
            return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));;
        }

        void updateCanvas(cv::Mat image, std::vector<cv::Point> convexHullPoints)
        {
            _canvas = image;
            _convexHullPoints = convexHullPoints;
        }
};