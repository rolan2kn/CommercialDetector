//
// Created by rkindela on 28-08-19.
//

#ifndef COMMERCIALDETECTOR_SIMILARITY_SEARCH_CONTROLLER_H
#define COMMERCIALDETECTOR_SIMILARITY_SEARCH_CONTROLLER_H
#include <iostream>
#include <vector>

#include "video_descriptor_controller.h"
using namespace std;


class SimilaritySearchController
{
private:
    string tv_path;
    string comm_path;
    string cache;
    vector<VideoDescriptorController*> video_list;
    vector<VideoDescriptorController*> commercial_list;

public:
    SimilaritySearchController(const string& _tv_desc_path, const string& _commercial_desc_path, const string& _cache);
    ~SimilaritySearchController();
    void execute();
    void fillData();
    void stepForwardAllCommercials();
    void resetAllCommercials();
    void kClosestAccordingToEuclidean(int k, const VideoDescriptorController* target_descriptor, vector<VideoDescriptorController*>& closest);
};


#endif //COMMERCIALDETECTOR_SIMILARITY_SEARCH_CONTROLLER_H
