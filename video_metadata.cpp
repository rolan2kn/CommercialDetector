//
// Created by rolo on 29/8/19.
//

#include "video_metadata.h"
#include "util.hpp"

VideoMetadata::VideoMetadata(const string& _name, int _frame_length, int _fps, int dps)
:name(_name), frame_length(_frame_length), frame_per_second(_fps),descriptors_per_second(dps)
{
    descriptors_per_second = (descriptors_per_second != 0) ? descriptors_per_second : 1;
    this->offset = frame_per_second/descriptors_per_second;
}

VideoMetadata::VideoMetadata()
:name(""), frame_length(0), frame_per_second(25),descriptors_per_second(3)
{

}

void VideoMetadata::toFile(const string& filename)
{
    if (!this->isValid())
        return;

    ofstream output_file(filename, ios::binary);

    if(output_file.is_open())
    {
        char _name[1024] = {'\0'};
        strncpy(_name, name.c_str(), name.size());
        output_file.write( (char *) &_name, sizeof(_name) );
        output_file.write( (char *) &frame_length, sizeof(int) );
        output_file.write( (char *) &frame_per_second, sizeof(int) );
        output_file.write( (char *) &descriptors_per_second, sizeof(int) );
    }
    output_file.close();
}

void VideoMetadata::fromFile(const string& filename)
{
    ifstream input_file(filename, ios::binary);

    if(input_file.is_open())
    {
        char _name[1024] = {'\0'};
        input_file.read( (char *) &_name, sizeof(_name) );
        this->name = string(_name);
        input_file.read( (char *) &frame_length, sizeof(int) );
        input_file.read( (char *) &frame_per_second, sizeof(int) );
        input_file.read( (char *) &descriptors_per_second, sizeof(int) );
        this->offset = frame_per_second/descriptors_per_second;
    }
    input_file.close();
}

string VideoMetadata::getNextDescriptorName(int currentDescriptor)
{
    int next = currentDescriptor + offset;

    if (next > frame_length)
        next = frame_length;

    string filename(this->name + "/" + std::to_string(next));
    if(existe_archivo(filename))
        return filename;
    return string();
}
