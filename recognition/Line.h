#pragma once

struct Line
{
   int angle;
   int rho;
   int hits;

   Line() {};
   Line(int angle, int rho, int hits) : angle(angle), rho(rho), hits(hits) {};
};