//
// Created by rolo on 29/8/19.
//

#ifndef COMMERCIALDETECTOR_VIDEO_DESCRIPTOR_CONTROLLER_H
#define COMMERCIALDETECTOR_VIDEO_DESCRIPTOR_CONTROLLER_H

#include <opencv2/core.hpp>

#include <iostream>
#include "video_metadata.h"

using namespace std;


class VideoDescriptorController {
private:
    VideoMetadata metadata;
    cv::Mat* current_descriptor;
    int descriptor_id;
public:
    VideoDescriptorController();
    VideoDescriptorController(const string& video_dir);
    VideoDescriptorController(const VideoMetadata& vmd, const cv::Mat& frame, int _desc_id);
    ~VideoDescriptorController();
    double getEuclideanDistance(const VideoDescriptorController& vdc) const;
    void toFile(const string& filename);
    bool fromFile(const string& filename);
    bool stepForward();
    bool restart();
};

#endif //COMMERCIALDETECTOR_VIDEO_DESCRIPTOR_CONTROLLER_H
