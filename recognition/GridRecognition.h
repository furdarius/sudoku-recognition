#pragma once

#include <math.h>
#include <vector>
#include "Photo.h"
#include "Angle.h"
#include <string>
#include "Line.h"
#include "Constants.h"
#include "Rectangle.h"
#include "Point.h"

using std::vector;
using std::string;

class GridRecognition
{
    protected:
        Photo& _photo;
        vector< Angle > angleCache;

        vector<vector<int > > Hough(int = 1, double = 0.3);
    public:
        GridRecognition(Photo& photo) : _photo(photo)
        {
            double theta = RAD * -90.0;

            for (int angle = 0; angle <= 180; ++angle) {
                angleCache.push_back(Angle(theta));
                theta += RAD;
            }
        };
        virtual ~GridRecognition() { };

        double getHorizontalAngle();
        Rectangle getGridCoords(int = 1, double = 0.2);
        vector< Point > getLinesCrossPoints(int = 1, double = 0.3);
        
        void splitOnDigits(vector< Point >, string);
};