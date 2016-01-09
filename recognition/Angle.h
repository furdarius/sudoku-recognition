#pragma once

#include <math.h>

struct Angle
{
    double angle, sin, cos;

    Angle()
    {
        throw("Angle(): args is not enough.");
    };

    Angle(double t)
    {
        angle = t;
        sin = ::sin(t);
        cos = ::cos(t);
    }
};