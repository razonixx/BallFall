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
        std::vector<std::vector<cv::Point>> _contours;
        cv::Point2f _initialPosition;

    public:
        cv::Point2f position;
        float milisecondsSinceLastCollision;
        float x0 = 0, y0 = 0;
        float v0X, v0Y;
        int timeToBounce = 1;

        std::vector<int> convexHull;
        

    public: 
        Ball(
            cv::Mat canvas,
            cv::Point2f _position, 
            int radius, cv::Scalar color, 
            float bounce, 
            float weight
            )
        {
            _canvas = canvas;
            position = _position;
            _radius = radius;
            _color = color;
            _bounce = bounce;
            _weight = weight;
            _gravity = 15;

            _initialPosition = _position;

            milisecondsSinceLastCollision = 0;
        }

        ~Ball()
        {

        }

        void tick()
        {
            if(!checkCollision())
            {
                std::cout << std::endl << std::endl << std::endl << std::endl << std::endl;

                milisecondsSinceLastCollision+=.01;

                _xImpulse = x0 + (v0X * milisecondsSinceLastCollision); // x=x0+vxt
                _yImpulse = y0 + 0.5 * (v0Y + (v0Y - _gravity * milisecondsSinceLastCollision)) * milisecondsSinceLastCollision; // https://openstax.org/books/university-physics-volume-1/pages/4-3-projectile-motion

                std::cout << "xImpulse: " << _xImpulse << std::endl;
                std::cout << "yImpulse: " << _yImpulse << std::endl;
                position.x += _xImpulse;
                position.y -= _yImpulse;

                cv::circle(_canvas, position, _radius, _color, cv::LineTypes::FILLED);

                std::cout << "Angle of movement: " << getAngleOfMovement() << std::endl;
                std::cout << "Ball Position: " << position.x << ", " << position.y << std::endl;
                std::cout << "Time since last collision: " << milisecondsSinceLastCollision << std::endl;
            }
            else
            {
                collide();
            }
            
        }

        bool checkCollision()
        {
            if(milisecondsSinceLastCollision < timeToBounce)
                return false;
            for(int i = 0; i < _contours.size(); i++)
                for(int j = 0; j < _contours[i].size(); j++)
                    if(distanceBetweenPointAndBall(_contours[i][j], position) < 4.f) // 2 is the best threshold
                        return true;
            if(((position.x + _radius) >= _canvas.cols || (position.y + _radius) >= _canvas.rows) || ((position.x + _radius) <= 0 || (position.y + _radius) <= 0))
                return true;
            return false;
        }

        void collide()
        {
            
            std::cout << "Collision detected. Angle before colliding: " << getAngleOfMovement() << std::endl;

            x0 = (v0X * cos(getAngleOfMovement())) * milisecondsSinceLastCollision*.001 * -1;
            y0 = (v0Y * sin(getAngleOfMovement())) * milisecondsSinceLastCollision*.001 * -1;


            milisecondsSinceLastCollision = 0;
            tick();
        }

        float getAngleOfMovement()
        {
            return atan2(_yImpulse, _xImpulse) * (180.0 / M_PI);
        }

        void debug()
        {
            //_yImpulse = -100;
            std::cout << "X impulse: " << _xImpulse << std::endl << "Y impulse: " << _yImpulse << std::endl;
            //std::cout << "Movement angle:" << getAngleOfMovement() << std::endl;

            position = _initialPosition;

        }

        // Ball position must be second parameter
        float distanceBetweenPointAndBall(cv::Point point, cv::Point ballPoint)
        {
            return sqrt(pow((point.x - (ballPoint.x + _radius)), 2) + pow((point.y - (ballPoint.y + _radius)), 2));;
        }

        void updateCanvas(cv::Mat image, std::vector<std::vector<cv::Point>> contours)
        {
            _canvas = image;
            _contours = contours;
        }
};