#pragma once

struct Rectangle
{
   int x, y;
   int width, height;

   Rectangle() {};
   Rectangle(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {};
};