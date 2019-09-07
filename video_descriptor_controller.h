//
// Created by rolo on 29/8/19.
//


#ifndef COMMERCIALDETECTOR_VIDEO_DESCRIPTOR_CONTROLLER_H
#define COMMERCIALDETECTOR_VIDEO_DESCRIPTOR_CONTROLLER_H

#include <opencv2/core.hpp>

#include <iostream>
#include "video_metadata.h"

using namespace std;


/**
 * VideoDescriptorController
 *
 * Es una clase diseñada para encapsular toda la logica asociada
 * a la manipulacion de descriptores de un mismo video
 *
 * los descriptores se almacenan codificando  * en su nombre el frame de video a que pertenecen
 * y todos estan en un directorio con el nombre de dicho video
 *
 * eso facilita iterar por los descriptores la informacion de video se maneja mediante los metadatos
 *  *
 * */
class VideoDescriptorController {
private:
    VideoMetadata metadata;                 // metadatos del video a que corresponde el descriptor
    cv::Mat current_descriptor;             // descriptor actual
    int descriptor_id;                      // numero de frame en el video al que corresponde el descriptor
    int rows;                               // filas del descriptor
    int cols;                               // columnas del descriptor
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
    bool anyDataRemains();
    inline const cv::Mat& getCurrentDescriptor() const {return this->current_descriptor;}
    inline const VideoMetadata& getMetadata() const {return this->metadata;}
    inline int getDescriptorId() const {return this->descriptor_id;}
};

#endif //COMMERCIALDETECTOR_VIDEO_DESCRIPTOR_CONTROLLER_H
