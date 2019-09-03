//
// Created by rkindela on 28-08-19.
//

#include "similarity_search_controller.h"

#include <fstream>
#include "util.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <map>

SimilaritySearchController::SimilaritySearchController(const string& _tv_desc_path, const string& _commercial_desc_path, const string& _cache)
:tv_path(_tv_desc_path), comm_path(_commercial_desc_path), cache(_cache), video_list(), commercial_list()
{

}

void SimilaritySearchController::fillData()
{
    vector<string> tv_videos = listar_directorios(tv_path);
    vector<string> commercial_videos = listar_directorios(comm_path);

    if(cache.empty())
    {
        cache = directorio_actual();
    }

    for (auto tvvid : tv_videos)
    {
        VideoDescriptorController* vdc = new VideoDescriptorController(tvvid);
        this->video_list.push_back(vdc);
    }
    for (auto comvid : commercial_videos)
    {
        VideoDescriptorController* vdc = new VideoDescriptorController(comvid);
        this->commercial_list.push_back(vdc);
    }
}

SimilaritySearchController::~SimilaritySearchController()
{
    for(VideoDescriptorController* vdc: video_list)
    {
        delete vdc;
    }
    for(VideoDescriptorController* vdc: commercial_list)
    {
        delete vdc;
    }
    video_list.clear();
    video_list.clear();
}

void SimilaritySearchController::execute()
{
}

void SimilaritySearchController::stepForwardAllCommercials()
{
    for(VideoDescriptorController* vdc: commercial_list)
    {
        vdc->stepForward();
    }
}

void SimilaritySearchController::resetAllCommercials()
{
    for(VideoDescriptorController* vdc: commercial_list)
    {
        vdc->restart();
    }
}

void SimilaritySearchController::kClosestAccordingToEuclidean(int k, const VideoDescriptorController* target_descriptor, vector<VideoDescriptorController*>& closest)
{
    int size = commercial_list.size();
    map<double, VideoDescriptorController*> distanceMapper;
    for(int i = 0; i < size; i++)
    {
        VideoDescriptorController* vdc = commercial_list[i];
        double euclideanDistance = target_descriptor->getEuclideanDistance(*vdc);

        while (!distanceMapper.insert(std::make_pair(euclideanDistance, vdc)).second)
        {
            euclideanDistance += 0.05;
        }
    }
    int i = 0;
    for(auto pair: distanceMapper)
    {
        if (i < k)
        {
            closest.push_back(pair.second);
        }
    }
    distanceMapper.clear();
}
