#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <stdlib.h>
#include <time.h> 
#include <iostream>
#include <cmath>

class Ball
{
    private:
        cv::Mat _canvas;
        int _radius;
        cv::Scalar _color;
        float _elasticity;
        float _gravity;
        std::vector<std::vector<cv::Point>> _contours;
        cv::Point2f _initialPosition;

        float _angleOfMovement;

    public:
        cv::Point2f position;
        cv::Point2f initialVelocity;
        cv::Point2f velocity;
        float milisecondsSinceLastCollision;
        int timeToBounce = .4;

        std::vector<int> convexHull;
        

    public: 
        Ball(
            cv::Mat canvas,
            cv::Point2f _position, 
            cv::Point2f _velocity,
            int radius, cv::Scalar color, 
            float elasticity
            )
        {
            std::cout << _velocity.x << std::endl;
            _canvas = canvas;
            position = _position;
            velocity = _velocity;
            _radius = radius;
            _color = color;
            _elasticity = elasticity;
            _gravity = .098;

            _initialPosition = _position;
            initialVelocity = _velocity;

            milisecondsSinceLastCollision = 800;
        }

        ~Ball()
        {

        }

        void tick()
        {
            if(position.x + _radius >= _canvas.cols)
            {
                keepBallInFrameX(true);
            }
            if(position.y + _radius >= _canvas.rows)
            {
                keepBallInFrameY(false);
            }
            if(position.x - _radius < 0)
            {
                keepBallInFrameX(false);
            }
            if(position.y - _radius <= 0)
            {
                keepBallInFrameY(true);
            }

            milisecondsSinceLastCollision+=.01;
            velocity.y += _gravity;

            position.x += velocity.x;
            position.y += velocity.y;

            _angleOfMovement = getAngleOfMovement();
            cv::circle(_canvas, position, _radius, _color, cv::LineTypes::FILLED);
            std::cout << std::endl << std::endl << std::endl << std::endl;
            std::cout << "Ball Position: " << position.x << ", " << position.y << std::endl;
            std::cout << "Ball Velocity: " << velocity.x << ", " << velocity.y << std::endl;
            std::cout << "Movement angle: " << _angleOfMovement << std::endl;
            
            if(checkCollision())
            {
                milisecondsSinceLastCollision = 0;
                if(isAnglePositiveX(_angleOfMovement))
                {
                    velocity.x = velocity.x - (velocity.x * _elasticity);
                }
                else
                {
                    velocity.x = velocity.x + (velocity.x * _elasticity);
                }

                if(isAnglePositiveY(_angleOfMovement))
                {
                    velocity.y = velocity.y + (velocity.y * _elasticity);
                }
                else
                {
                    velocity.y = velocity.y - (velocity.y * _elasticity);
                }
            }
        }

        bool checkCollision()
        {
            if(milisecondsSinceLastCollision > timeToBounce)
            {
                for(int i = 0; i < _contours.size(); i++)
                    for(int j = 0; j < _contours[i].size(); j++)
                        if(floor(euclideanDist(_contours[i][j], position)) < 4)
                        {
                            return true;
                        }
            }
            return false;
        }

        float getAngleOfMovement()
        {
            //return cv::fastAtan2(-velocity.y, velocity.x);
            return atan2(-velocity.y, velocity.x) * (180.0 / M_PI);
        }

        void debug()
        {
            position = _initialPosition;
            velocity = initialVelocity;

        }

        // Ball position must be second parameter
        float euclideanDist(cv::Point p1, cv::Point p2) {
            cv::Point diff = p1 - p2;
            return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
        }

        void updateCanvas(cv::Mat image, std::vector<std::vector<cv::Point>> contours)
        {
            _canvas = image;
            _contours = contours;
        }


        void keepBallInFrameX(bool right)
        {
            if(right)
            {
                position.x = _canvas.cols - _radius - 2;
                velocity.x = initialVelocity.x;
            }
            else
            {
                position.x = _radius + 2;
                velocity.x = -initialVelocity.x;
            }
            
        }

        void keepBallInFrameY(bool top)
        {
            if(top)
            {
                position.y = _radius + 2;
                velocity.y = 0;
            }
            else
            {
                position.y = _canvas.rows - _radius - 2;
                velocity.y = -4;
            }
            
        }

        bool isAnglePositiveX(float angleOfMovement)
        {
            if(angleOfMovement >= 270 && angleOfMovement <= 90)
                return false; //Positive movement in y
            if(angleOfMovement >= 90 && angleOfMovement <= 270)
                return true; //Negative movement in y
            if(angleOfMovement <= -270 && angleOfMovement >= -90)
                return true; //Positive movement in y
            if(angleOfMovement <= -90 && angleOfMovement >= -270)
                return false; //Negative movement in y
            return false;
        }

        bool isAnglePositiveY(float angleOfMovement)
        {
            if(angleOfMovement >= 0 && angleOfMovement <= 180)
                return true; //Positive movement in y
            if(angleOfMovement >= 180 && angleOfMovement <= 360)
                return false; //Negative movement in y
            if(angleOfMovement <= 0 && angleOfMovement >= -180)
                return false; //Positive movement in y
            if(angleOfMovement <= -180 && angleOfMovement >= -360)
                return true; //Negative movement in y
            return false;
        }
};