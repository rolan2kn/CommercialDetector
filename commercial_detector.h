//
// Created by rkindela on 28-08-19.
//

#ifndef COMMERCIALDETECTOR_COMMERCIAL_DETECTOR_H
#define COMMERCIALDETECTOR_COMMERCIAL_DETECTOR_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "video_descriptor_controller.h"

using namespace std;
/**
 * CommercialDetector
 *
 * Clase que procesa la informacion de cercania creada y almacenada por SimilaritySearchController y determina si
 * aparece un comercial en un video de tv, cuando a parece y por cuanto tiempo.
 *
 * */

struct DetectionInfo
{
    long video_init_frame;
    long com_init_frame;
    long current_frame;
    long com_frame;
    VideoDescriptorController* com_dc;

    DetectionInfo(const string& com_name);
    void update(long video_frame, long com_frame);
    void reset();
    inline bool isValid() {return current_frame != -1 && com_frame != -1; }
};

class CommercialDetector {
private:
    ifstream closenessInfo;
    string info_filename;
    ofstream result;
    float tolerance;
    string cache;
    string last_video;

    map<string, int> video_mapper;
    map<string, int> commercial_mapper;
    vector<DetectionInfo> detectionInfoList;
    VideoDescriptorController* video_dc;
    int** voting_table;
public:
    CommercialDetector(const string& cache, const string& closenessInfo, float tolerance = 0.75f);
    ~CommercialDetector();
    void execute();
    void init();
    void reset();
    void computeVote(const string& video_segment, const string& com_segment, long frame_no, long com_no);
    float computeAccuracy(const DetectionInfo& di, long com_no);
    void reportingResults(const DetectionInfo& di, long frame_no, long com_no);
};


#endif //COMMERCIALDETECTOR_COMMERCIAL_DETECTOR_H
