//
// Created by rolo on 29/8/19.
//

#include "video_descriptor_controller.h"
#include <opencv2/imgproc.hpp>
#include "util.hpp"


VideoDescriptorController::VideoDescriptorController(const string& video_dir)
:metadata(), current_descriptor(new cv::Mat), descriptor_id(0)
{
    metadata.fromFile(video_dir + "/" + VideoMetadata::metadataFileName());
    descriptor_id = -metadata.offset; // to start in 0
    this->stepForward();
}

VideoDescriptorController::VideoDescriptorController()
        :metadata(), current_descriptor(new cv::Mat), descriptor_id(0)
{
    
}

VideoDescriptorController::VideoDescriptorController(const VideoMetadata& vmd, const cv::Mat& frame, int _desc_id)
:metadata(vmd), current_descriptor(new cv::Mat), descriptor_id(_desc_id)
{
    cv::resize(frame, *current_descriptor, cv::Size(10,10));
}

VideoDescriptorController::~VideoDescriptorController()
{
    if(current_descriptor != nullptr)
        delete current_descriptor;
    current_descriptor = nullptr;
}

double VideoDescriptorController::getEuclideanDistance(const VideoDescriptorController& vdc) const
{
    if(vdc.current_descriptor == nullptr || this->current_descriptor == nullptr)
        return -1;

    return cv::norm(*(this->current_descriptor), *(vdc.current_descriptor));
}

void VideoDescriptorController::toFile(const string& filename)
{
    if(this->current_descriptor == nullptr || this->current_descriptor->empty())
        return;
    
    ofstream descriptor_file(filename, ios::binary);
    if (descriptor_file.is_open())
    {
        descriptor_file.write( (char *) this->current_descriptor, sizeof(cv::Mat) );
    }

    descriptor_file.close();
}

bool VideoDescriptorController::fromFile(const string& filename)
{
    if (filename.empty())
        return false;

    string name = basename(filename);
    if(!metadata.isValid())
    {
        string path = filename.substr(0, filename.length()-name.length());
        metadata.fromFile(path + "/" + VideoMetadata::metadataFileName());
    }
    descriptor_id = parse_int(basename(filename));
    ifstream desc_file(filename, ios::binary);
    bool result = false;
    if(desc_file.is_open())
    {
        desc_file.read((char *) this->current_descriptor, sizeof(cv::Mat ) );
        result = true;
    }
    desc_file.close();

    return result;
}

bool VideoDescriptorController::stepForward()
{
    string next_desc_name = metadata.getNextDescriptorName(descriptor_id);
    return this->fromFile(next_desc_name);
}

bool VideoDescriptorController::restart()
{
    this->descriptor_id = -metadata.offset; // retrocedo para caer en el descriptor cero
    return this->stepForward();
}
