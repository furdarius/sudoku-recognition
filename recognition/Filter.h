#pragma once

#include "Photo.h"

class Filter
{
    public:
        static void Binarization(Photo&, double = 0.5);
        static void AdaptiveBinarization(Photo&, int = 5, double = 0.06);
};