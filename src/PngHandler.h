#ifndef PNGHANDLER_H
#define PNGHANDLER_H
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION


#include <iostream>
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#include <random>
#include <math.h>
#include <limits.h>
#include <unordered_map>

struct ImagePart {
    int initialIIndex;
    int initialJIndex;
    int height;
    int width;
};

struct Pixel {
    int pixelCoordx;
    int pixelCoordy;

    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct MeanObject {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    float meanChange = 1000000000;

    std::vector<std::pair<int,int>> memberPixels;

};

struct Part {
    std::vector<MeanObject> means;
    ImagePart* OperationArea;
};

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

float convergenceThreshold = 1.0;


class PngHandler {
private:
    uint8_t* _rgb_image;
    uint8_t* _rgb_image_ref;
    
public:
    std::vector<ImagePart> imageParts;
    int _width;
    int _height;
    int _bpp;
    PngHandler(char* filename) {
        _rgb_image = stbi_load(filename,&this->_width,&this->_height,&this->_bpp,3);
        _rgb_image_ref = stbi_load(filename,&this->_width,&this->_height,&this->_bpp,3);
    }
    ~PngHandler() {
        delete[] _rgb_image;
    }
    void write(char* filename) {
        stbi_write_png(filename, this->_width,
                                 this->_height,
                                 3,
                                 this->_rgb_image,
                                 this->_width*3);
    }

    void setPixel(int i, int j, uint8_t r, uint8_t g, uint8_t b) {
        _rgb_image[(j-1)*3 + _width*(i -1)*3] = r;
        _rgb_image[(j-1)*3 + _width*(i -1)*3 + 1] = g;
        _rgb_image[(j-1)*3 + _width*(i -1)*3 + 2] = b;

    }

    void divide(int divideHeight, int divideWidth) {
        int heightPartNo = _height/divideHeight;
        int widthPartNo = _width/divideWidth;

        for(int i=0; i<heightPartNo; i++) {
            for(int j=0; j<widthPartNo; j++) {
                ImagePart part;
                part.height = divideHeight;
                part.width = divideWidth;
                part.initialIIndex = i*divideHeight + 1;
                part.initialJIndex = j*divideWidth + 1;
                imageParts.push_back(part);
            }
        }

    }

    void imagePartDraw(uint8_t r, uint8_t g, uint8_t b) {
        ImagePart p = imageParts[1];

        for(int i=0; i<p.height; i++) {
            for(int j=0; j<p.width; j++) {
                setPixel(i + p.initialIIndex, j + p.initialJIndex, r, g, b);
            }
        }
    }

    Color getColor(int i, int j) {
        Color result;
        result.r = _rgb_image[(j-1)*3 + _width*(i-1)*3];
        result.g = _rgb_image[(j-1)*3 + _width*(i-1)*3 + 1];
        result.b = _rgb_image[(j-1)*3 + _width*(i-1)*3 + 2];

        return result;
    }

    Color getColorFromRef(int i, int j) {
        Color result;
        result.r = _rgb_image_ref[(j-1)*3 + _width*(i-1)*3];
        result.g = _rgb_image_ref[(j-1)*3 + _width*(i-1)*3 + 1];
        result.b = _rgb_image_ref[(j-1)*3 + _width*(i-1)*3 + 2];

        return result;
    }

};

#endif /* PNGHANDLER_H */