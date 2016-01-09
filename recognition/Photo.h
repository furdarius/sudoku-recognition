#pragma once
#include <iostream>
#include <string>
#include <Magick++.h>

using std::string;
using Magick::Image;
using Magick::PixelPacket;

class Photo
{
	protected:
        Image _image;

	public:
		Photo(string);
		virtual ~Photo() { };

        int width(void);
        int height(void);

        Magick::PixelPacket* pixels(void);
        void syncPixels(void);

        void save(string);

        void image(Image&);

        void resize(int, int);
        void rotate(double);
        void crop(int, int, int, int);
};