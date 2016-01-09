#include <Magick++.h>
#include <vector>
#include "Filter.h"
#include "Color.h"


using Magick::PixelPacket;
using Magick::ColorRGB;
using Magick::Image;
using Magick::Geometry;
using std::vector;

void Filter::Binarization(Photo& photo, double threshold)
{
    int width = photo.width();
    int height = photo.height();

    PixelPacket *pixels = photo.pixels();

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            ColorRGB color = pixels[width * y + x];         
            pixels[width * y + x] = Color::toBrightness(color) > threshold ? Color::White : Color::Black;
        }
    }

    photo.syncPixels();
}

void Filter::AdaptiveBinarization(Photo& photo, int rect, double threshold)
{
    int width = photo.width();
    int height = photo.height();

    PixelPacket *pixels = photo.pixels();
    
    Image result(Geometry(width, height), Color::White);

    PixelPacket *res_pixels = result.getPixels(0, 0, width, height);
    
	int count = (rect * 2 + 1) * (rect * 2 + 1);
    
    vector<double> buf(width * height);
    for (size_t y = 0; y < height; ++y) {
        int yOffset = y * width;
		for (size_t x = 0; x < width; ++x) {
            int offset = yOffset + x;
            buf[offset] = Color::toBrightness(ColorRGB(pixels[offset]));
        }
    }

    for (size_t y = rect + 1; y < height - rect; ++y) {
        int yOffset = y * width;
		for (size_t x = rect + 1; x < width - rect; ++x) {
            int offset = yOffset + x;

            double avr = 0;
            for (size_t i = 0; i < rect * 2 + 1; ++i) {
                for (size_t j = 0; j < rect * 2 + 1; ++j) {
                    avr += buf[(y - rect - 1 + i) * width + (x - rect - 1 + j)];
                }
            }
            avr /= count;
 
            res_pixels[offset] = (buf[offset] > (avr - threshold)) ? Color::White : Color::Black;
        }
    }

    result.syncPixels();
    photo.image(result);
}