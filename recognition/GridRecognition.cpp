#include <Magick++.h>
#include <vector>
#include <string>
#include <sstream>
#include "GridRecognition.h"
#include "Constants.h"
#include "Photo.h"
#include "Rectangle.h"

using Magick::PixelPacket;
using std::vector;

vector<vector<int > > GridRecognition::Hough(int rect, double threshold)
{
    size_t width = _photo.width();
    size_t height = _photo.height();

    PixelPacket *pixels = _photo.pixels();

	int count = (rect * 2 + 1) * (rect * 2 + 1);
    int STEP = rect * 2 + 1;

    int D = sqrt(double(width * width + height * height));
    int D_Offset = D / 2;

    vector<vector<int > > accum(D + D_Offset + 1, vector< int >(181, 0));
    int _max = 0;
    int best_angle = -1, best_rho = -1;
    for (size_t x = rect; x < width - rect; x += STEP) {
        for (size_t y = rect; y < height - rect; y += STEP) {
            bool is_black = false;
            
            int sum = 0;
            for (int i = y - rect; i <= y + rect; ++i) {
				for (int j = x - rect; j <= x + rect; ++j) {
                    bool is_black = pixels[width * i + j].red == 0;

                    if (is_black) {
					    sum++;
					}
				}
			}

            double avr = (double)sum / (double)count;
            if (avr >= threshold) {	// black
				is_black = true;
            }
            
            if (!is_black) {
                continue;
            }

            for (int angle = 0; angle <= 180; ++angle) {
                int rho = abs((x * angleCache[angle].cos) + (y * angleCache[angle].sin) + D_Offset);
                
                accum[rho][angle]++;

                if (accum[rho][angle] > _max) {
                    _max = accum[rho][angle];

                    best_angle = angle;
                    best_rho = rho;
                }
            }
        }
    }

    return accum;
};


double GridRecognition::getHorizontalAngle()
{
    vector<vector<int > > accum = Hough(0);
  
    Line best(-1, -1, -1);
    for (int rho = 0; rho < accum.size(); ++rho) {
        for (int angle = 0; angle < accum[rho].size(); ++angle) {
            if (best.hits < accum[rho][angle]) {
                best.angle = angle;
                best.rho = rho;
                best.hits = accum[rho][angle];
            }
        }
    }


    if (best.angle == -1) {
        throw("Angle for rotation not found");
    }

    double thetaHotPoint = ((PI / 180) * -90.0) + (PI / 180) * best.angle;
    double rotate_angle = (90 - abs(thetaHotPoint) * (180 / PI)) * (thetaHotPoint < 0 ? -1 : 1);

    if (rotate_angle > 45) {
        rotate_angle -= 90;
    }

    return rotate_angle;
};


Rectangle GridRecognition::getGridCoords(int rect, double threshold)
{
    size_t width = _photo.width();
    size_t height = _photo.height();

    PixelPacket *pixels = _photo.pixels();

    int count = (rect * 2 + 1) * (rect * 2 + 1);
    int STEP = rect * 2 + 1;

    std::vector< int > white_lines(width, 0);
	int longestwhiteLen = 0;
    for (size_t x = rect; x < width - rect; x += STEP) {
        int cross_count = 0;
        int whiteLen = 0;
        for (size_t y = rect; y < height - rect; y += STEP) {
            int sum = 0;
            for (int i = y - rect; i <= y + rect; ++i) {
				for (int j = x - rect; j <= x + rect; ++j) {
                    bool is_black = pixels[width * i + j].red == 0;

                    if (is_black) {
					    sum++;
					}
				}
			}

            double avr = (double)sum / (double)count;
            if (avr <= threshold) {	// white
				whiteLen++;
            } else {            // black
                if (longestwhiteLen < whiteLen) {
					longestwhiteLen = whiteLen;
                }

				whiteLen = 0;
                cross_count++;
			}
        }

        white_lines[x] = whiteLen;
    }


    int left_x = -1;
    int maxValue = 0;
	for (int x = 1 + width / 2; x >= 0; --x) {
		if (maxValue < white_lines[x]) {
            maxValue = white_lines[x];
			left_x = x;
		}
    }

    int right_x = -1;
    maxValue = 0;
	for (int x = width / 2; x < width; ++x) {
		if (maxValue < white_lines[x]) {
            maxValue = white_lines[x];
			right_x = x;
		}
    }

    white_lines.clear();
    white_lines.resize(height, 0);
	longestwhiteLen = 0;
    for (size_t y = rect; y < height - rect; y += STEP) {
        int cross_count = 0;
        int whiteLen = 0;
        for (size_t x = rect; x < width - rect; x += STEP) {
            int sum = 0;

            for (int i = y - rect; i <= y + rect; ++i) {
				for (int j = x - rect; j <= x + rect; ++j) {
                    bool is_black = pixels[width * i + j].red == 0;

                    if (is_black) {
					    sum++;
					}
				}
			}

            double avr = (double)sum / (double)count;
            if (avr <= threshold) {	// white
				whiteLen++;
            } else {            // black
                if (longestwhiteLen < whiteLen) {
					longestwhiteLen = whiteLen;
                }

				whiteLen = 0;
                cross_count++;
			}
        }


        white_lines[y] = whiteLen;
    }


    int bottom_y = -1;
    maxValue = 0;
    for (int y = height / 2 + 1; y < height - 1; y++) {
	    if (maxValue < white_lines[y]) {
            maxValue = white_lines[y];
		    bottom_y = y;
	    }
    }

    int top_y = -1;
    maxValue = 0;
    for (int y = height / 2; y > 0; --y) {
        if (maxValue < white_lines[y]) {
            maxValue = white_lines[y];
			top_y = y;
		}
    }
    
    return Rectangle(left_x, top_y, right_x - left_x, bottom_y - top_y);
};


vector< Point > GridRecognition::getLinesCrossPoints(int rect, double threshold)
{
    size_t width = _photo.width();
    size_t height = _photo.height();

    PixelPacket *pixels = _photo.pixels();

    vector<vector<int > > accum = Hough(rect, threshold);

    int D_Offset = sqrt(double(width * width + height * height)) / 2;

    std::vector< int > vertical_best(width, 0);
    for (size_t x = 0; x < width; x++) {
        int sum = 0;
        for (size_t y = 0; y < height; y++) {
            for (int angle = 80; angle <= 100; ++angle) {
                int rho = abs((x * angleCache[angle].cos) + (y * angleCache[angle].sin) + D_Offset);
                
                sum += accum[rho][angle];
            }
        }
        vertical_best[x] = sum;
    }
    
    int max_val = 0;
    int left_x = 0;
    for (size_t x = 0; x < width / 10; x++) {
        if (vertical_best[x] > max_val) {
            max_val = vertical_best[x];
            left_x = x;
        }
    }

    max_val = 0;
    int right_x = 0;
    for (size_t x = width - 1; x >= width - width / 10; x--) {
        if (vertical_best[x] > max_val) {
            max_val = vertical_best[x];
            right_x = x;
        }
    }


    int grid_width = right_x - left_x;
    int slot_width = grid_width / 9;

    std::vector<int> x_coords;
    x_coords.push_back(left_x);
    for (int part = 1; part < 9; ++part) {
        int _max_val = 0;
        int part_x = 0;
        for (size_t x = left_x + part * slot_width + 1; x < (part + 1) * slot_width && x < right_x; ++x) {
            if (vertical_best[x] > _max_val) {
                _max_val = vertical_best[x];
                part_x = x;
            }
        }
        x_coords.push_back(part_x);
    }
    x_coords.push_back(right_x);


    std::vector< int > horizontal_best(height, 0);
    for (size_t y = 0; y < height; y++) {
        int sum = 0;
        for (size_t x = 0; x < width; x++) {
            for (int angle = 0; angle <= 10; ++angle) {
                int rho = abs((x * angleCache[angle].cos) + (y * angleCache[angle].sin) + D_Offset);
                
                sum += accum[rho][angle];
            }

            for (int angle = 170; angle <= 180; ++angle) {
                int rho = abs((x * angleCache[angle].cos) + (y * angleCache[angle].sin) + D_Offset);
                
                sum += accum[rho][angle];
            }
        }
        horizontal_best[y] = sum;
    }

    max_val = 0;
    int top_y = 0;
    for (size_t y = 0; y < height / 10; ++y) {
        if (horizontal_best[y] > max_val) {
            max_val = horizontal_best[y];
            top_y = y;
        }
    }

    max_val = 0;
    int bottom_y = 0;
    for (size_t y = height - 1; y >= height - height / 10; --y) {
        if (horizontal_best[y] > max_val) {
            max_val = horizontal_best[y];
            bottom_y = y;
        }
    }


    int grid_height = bottom_y - top_y;
    int slot_height = grid_height / 9;

    std::vector<int> y_coords;
    y_coords.push_back(top_y);
    for (int part = 1; part < 9; ++part) {
        int _max_val = 0;
        int part_y = 0;
        for (size_t y = (part * (height / 10)) + 1; y < ((part + 1) * (height / 10)); ++y) {
            if (horizontal_best[y] > _max_val) {
                _max_val = horizontal_best[y];
                part_y = y;
            }
        }
        y_coords.push_back(part_y);
    }
    y_coords.push_back(bottom_y);


    if (x_coords.size() != y_coords.size()) {
        throw("x_coords.size() != y_coords.size()");
    }

    vector< Point > result(x_coords.size());
    for (int i = 0; i < x_coords.size(); ++i) {
        result[i] = Point(x_coords[i], y_coords[i]);
    };

    return result;
};


void GridRecognition::splitOnDigits(vector< Point > points, string folder)
{
    size_t width = _photo.width();
    size_t height = _photo.height();

    PixelPacket *pixels = _photo.pixels();

    for (int i = 0; i < points.size() - 1; ++i) {
        int x1 = points[i].x;
        int x2 = points[i + 1].x;
        for (int j = 0; j < points.size() - 1; ++j) {
            int y1 = points[j].y;
            int y2 = points[j + 1].y;

            int digit_width = x2 - x1;
            int digit_height = y2 - y1;
            Magick::Image digit(Magick::Geometry(digit_width, digit_height), Magick::ColorRGB(1, 1, 1));
            Magick::PixelPacket *digit_pixels = digit.getPixels(0, 0, digit_width, digit_height);
            
            for (int x = x1; x < x2; ++x) {
                for (int y = y1; y < y2; ++y) {
                    //pixels[width * y + x] = Magick::ColorRGB(0, 0, 0.5);

                    digit_pixels[digit_width * (y - y1) + (x - x1)] = pixels[width * y + x];
                }
            }
            digit.syncPixels();
            
            
            std::ostringstream name;
            name << folder << "/digit_" << std::to_string((long long)i) << "_" << std::to_string((long long)j) << ".bmp";
            
            digit.write(name.str());
        }
    }
}