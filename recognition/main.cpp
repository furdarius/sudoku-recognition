#include <iostream>
#include <time.h>
#include <Magick++.h>
#include "Photo.h"
#include "Filter.h"
#include "GridRecognition.h"
#include "Rectangle.h"

int main(int argc, char **argv)
{
    try {
        clock_t begin_time, summary_time = clock();

        begin_time = clock();
        Photo sudoku("9.jpg");
        std::cout << "Open image: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;

        // 1) Resize
        begin_time = clock();
        sudoku.resize(512, 512);
        std::cout << "Resizing: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;

        // 2) Binarization
        begin_time = clock();
        Filter::AdaptiveBinarization(sudoku, 4, 0.04);
        std::cout << "Adaptive Binarization: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;

        GridRecognition recognition(sudoku);

        // 3) Rotate
        begin_time = clock();
        double angle = recognition.getHorizontalAngle();
        std::cout << "getHorizontalAngle: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;
        sudoku.rotate(angle);
        std::cout << "getHorizontalAngle + Rotation: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;

        // 4) Crop
        begin_time = clock();
        Rectangle grid = recognition.getGridCoords();
        std::cout << "getGridCoords: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;
        sudoku.crop(grid.x, grid.y, grid.width, grid.height);
        std::cout << "getGridCoords + crop: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;

        // 5) Split digits
        begin_time = clock();
        vector< Point > points = recognition.getLinesCrossPoints();
        std::cout << "getLinesCrossPoints: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;
        recognition.splitOnDigits(points, "digits");
        std::cout << "getLinesCrossPoints + splitOnDigits: " << double(clock() - begin_time) / CLOCKS_PER_SEC << " sec." << std::endl;

        sudoku.save("test.jpg");

        std::cout << "Summary: " << double(clock() - summary_time) / CLOCKS_PER_SEC << " sec." << std::endl;
    } catch(Magick::Exception &error_) { 
        std::cout << "Caught exception: " << error_.what() << std::endl; 

        system("pause");
        return 1; 
    }

    system("pause");
    return 0;
}