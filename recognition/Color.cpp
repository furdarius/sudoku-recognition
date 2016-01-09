#include "Color.h"

ColorRGB Color::Black   = ColorRGB(0, 0, 0);
ColorRGB Color::White   = ColorRGB(1, 1, 1);
ColorRGB Color::Red     = ColorRGB(1, 0, 0);
ColorRGB Color::Green   = ColorRGB(0, 1, 0);
ColorRGB Color::Blue    = ColorRGB(0, 0, 1);

double Color::toBrightness(double red, double green, double blue)
{
    return 0.299 * red
         + 0.587 * green
         + 0.114 * blue;
};

double Color::toBrightness(ColorRGB& color)
{
    return toBrightness(color.red(), color.green(), color.blue());
}