#pragma once

#include <Magick++.h>

using Magick::ColorRGB;

class Color
{
    public:
        static ColorRGB Black, White, Red, Green, Blue;

        static double toBrightness(double, double, double);
        static double toBrightness(ColorRGB&);
};
