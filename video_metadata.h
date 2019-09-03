//
// Created by rolo on 29/8/19.
//

#ifndef COMMERCIALDETECTOR_VIDEO_METADATA_H
#define COMMERCIALDETECTOR_VIDEO_METADATA_H

#include <iostream>
#include <fstream>
using namespace std;

struct VideoMetadata
{
    string name;
    int frame_length;
    int frame_per_second;
    int descriptors_per_second;
    int offset;
    VideoMetadata();
    VideoMetadata(const string& _name, int _frame_length, int _fps, int dps);
    void toFile(const string& filename);
    void fromFile(const string& filename);
    string getNextDescriptorName(int currentDescriptor);
    inline bool isValid(){return (!name.empty() && frame_length > 0 && frame_per_second > 0);}
    static string metadataFileName(){return "metadata";}
};


#endif //COMMERCIALDETECTOR_VIDEO_METADATA_H
