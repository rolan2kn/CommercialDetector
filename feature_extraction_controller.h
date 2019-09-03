//
// Created by rkindela on 28-08-19.
//

#ifndef COMMERCIALDETECTOR_FEATURE_EXTRACTION_CONTROLLER_H
#define COMMERCIALDETECTOR_FEATURE_EXTRACTION_CONTROLLER_H

#include <iostream>
using namespace std;

class FeatureExtractionController {
private:
    string tv_path;
    string commercial_path;
    string cache;
    int descriptors_by_second;
private:
    void create_descriptors(const string& video_path, const string& outputdir = "");
public:
    FeatureExtractionController(const string& _tv_path, const string& _commercial_path, const string& _cache, int descriptors_per_second = 3);
    ~FeatureExtractionController();
    void execute();
};


#endif //COMMERCIALDETECTOR_FEATURE_EXTRACTION_CONTROLLER_H
