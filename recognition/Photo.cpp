#include <Magick++.h>
#include "Photo.h"
#include "Color.h"

using Magick::Geometry;

Photo::Photo(string name)
{
	_image = Magick::Image(name);
};

int Photo::width()
{
    return _image.columns();
};

int Photo::height()
{
    return _image.rows();
};

PixelPacket* Photo::pixels()
{
    return _image.getPixels(0, 0, width(), height());
};

void Photo::syncPixels(void)
{
    _image.syncPixels();
};

void Photo::save(string name)
{
    _image.write(name);
};

void Photo::image(Image& image)
{
    _image = image;
};

void Photo::resize(int w, int h)
{
    _image.resize(Geometry(w, h));
};

void Photo::rotate(double angle)
{
    _image.rotate(angle);
};

void Photo::crop(int cropX, int cropY, int cropWidth, int cropHeight)
{
    // image.crop(Magick::Geometry(right_x - left_x, height, left_x, 0));

    // FIXME: Костыль, потому что Magick криво кропает, ну или я не умею его готовить...

    PixelPacket *_pixels = pixels();

    Image res(Magick::Geometry(cropWidth, cropHeight), Color::White);
    PixelPacket *res_pixels = res.getPixels(0, 0, cropWidth, cropHeight);

    int fullWidth = width();
    for (size_t x = 0; x < cropWidth; ++x) {
        for (size_t y = 0; y < cropHeight; ++y) {     
            res_pixels[cropWidth * y + x] = _pixels[fullWidth * (y + cropY + 1) + (x + cropX + 1)];
        }
    }

    res.syncPixels();
    _image = res;
};