#ifndef KMEANSIMAGECOMPRESS_H
#define KMEANSIMAGECOMPRESS_H

#include "PngHandler.h"
#include <thread>

class KMeansImageCompress {
private:
    PngHandler *pngHandler;
    std::vector<Part> parts;
    std::vector<std::thread> threads;
    int c, k;

    void populateMeanMembers(Part& part) {
        
        for(int i=0; i<part.OperationArea->height; i++) {
            int realI = part.OperationArea->initialIIndex + i;
            for(int j=0; j<part.OperationArea->width; j++) {
                int realJ = part.OperationArea->initialJIndex + j;

                Color colorOfPixel = pngHandler->getColor(realI, realJ);
                float distance = 100000000;
                int meanIndex;
                for(int k=0; k<part.means.size(); k++) {
                    float d = giveDistance(colorOfPixel.r,
                                           colorOfPixel.g,
                                           colorOfPixel.b,
                                           part.means[k].r,
                                           part.means[k].g,
                                           part.means[k].b);
                    if(d < distance) {
                        distance = d;
                        meanIndex = k;
                    }

                }
                std::pair<int,int> pixelCoords;
                pixelCoords.first = realI;
                pixelCoords.second = realJ;
                part.means[meanIndex].memberPixels.push_back(pixelCoords);

            }
        }
    }

    bool isSystemConverging(Part& part) {
        bool result = true;

        for(const MeanObject& m : part.means) {
            if(m.meanChange > convergenceThreshold) {
                result = false;
            }
        }
        return true;
    }

    void clearMeanMembers(Part& part) {
        for(MeanObject& m : part.means) {
            m.memberPixels.clear();
        }
    }

    void kMeansForPart(Part& part) {

        for(int i=0; i<40; i++) {
            populateMeanMembers(part);
            calculateMeanForPart(part);
            clearMeanMembers(part);
        }

        populateMeanMembers(part);

        for(MeanObject& mean : part.means) {
            writePixels(mean);
        }

    }

    void writePixels(MeanObject& meanObject) {


        for(std::pair<int,int> pixel: meanObject.memberPixels) {
            pngHandler->setPixel(pixel.first, pixel.second,
                                 meanObject.r,
                                 meanObject.g,
                                 meanObject.b);
        }
    }

    void calculateMeanForPart(Part& part) {
        for(MeanObject& m: part.means) {
            calculateMean(m);
        }
    }

    void calculateMean(MeanObject& meanObject) {

        if(meanObject.memberPixels.size() > 0) {
            int r = 0;
            int g = 0;
            int b = 0;

            for(std::pair<int,int>& pair : meanObject.memberPixels) {
                Color colorOfPixel = pngHandler->getColor(pair.first, pair.second);

                r += static_cast<int>(colorOfPixel.r);
                g += static_cast<int>(colorOfPixel.g);
                b += static_cast<int>(colorOfPixel.b);
            }

            r /= meanObject.memberPixels.size();
            g /= meanObject.memberPixels.size();
            b /= meanObject.memberPixels.size();


            float d = giveDistance(r,g,b, meanObject.r, meanObject.g, meanObject.b);
            meanObject.meanChange = d;
            meanObject.r = r;
            meanObject.g = g;
            meanObject.b = b;

        }
    }



    float giveDistance(int r1, int g1, int b1, int r2, int g2, int b2) {
        return sqrt(pow((r1 - r2),2) + pow((b1 - b2),2) + pow((g1 - g2),2));
    };

    void initializeMeans() {

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<uint8_t> dist(0,255);

        for(ImagePart& ip : pngHandler->imageParts) {
            Part part;
            part.OperationArea = &ip;

            for(int i=0; i<k; i++) {
                MeanObject mean;
                mean.r = dist(mt);
                mean.g = dist(mt);
                mean.b = dist(mt);

                part.means.push_back(mean);
            }

            parts.push_back(part);
        }
    }


public:

    float error;

    KMeansImageCompress(char* filename, int c, int k) {
        pngHandler = new PngHandler(filename);
        this->c = c;
        this->k = k;
    }

    ~KMeansImageCompress() {
        delete pngHandler;
    }

    void initialize() {
        pngHandler->divide(c,c);
        initializeMeans();
    }

    void calculateError() {

        float err = 0;

        for(int i=1; i<pngHandler->_height+1; i++) {
            for(int j=1; j<pngHandler->_width+1; j++) {
                Color colorCompressed = pngHandler->getColor(i,j);
                Color colorReference = pngHandler->getColorFromRef(i,j);

                float dist = giveDistance(colorCompressed.r,
                                          colorCompressed.g,
                                          colorCompressed.b,
                                          colorReference.r,
                                          colorReference.g,
                                          colorReference.b);

                err += dist;
            }
        }

        err = err/(pngHandler->_height*pngHandler->_width);

        error = err/50;
    }

    void run() {
        for(Part& part: parts) {
            threads.push_back(std::thread([this, &part] {kMeansForPart(part);}));
        }

        for(auto &th : threads) {
            th.join();
        }
    }

    float getError() {
        return error;
    }

    void write(char* filename) {
        pngHandler->write(filename);
    }
};


#endif