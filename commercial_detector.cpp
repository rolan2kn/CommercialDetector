//
// Created by rkindela on 28-08-19.
//

#include "commercial_detector.h"
#include "video_descriptor_controller.h"
#include "util.hpp"
#include <map>

DetectionInfo::DetectionInfo(const string& com_name)
:video_init_frame(-1), current_frame(-1), com_frame(-1), com_init_frame(-1)
{
    com_dc = new VideoDescriptorController(com_name);
}

void DetectionInfo::update(long video_frame, long _com_frame)
{
    current_frame = video_frame;
    if(video_init_frame == -1)
        video_init_frame = video_frame;

    com_frame = _com_frame;
    if(com_init_frame == -1)
        com_init_frame = com_frame;
}

void DetectionInfo::reset()
{
    video_init_frame = -1;
    com_init_frame = -1;
    current_frame = -1;
    com_frame = -1;
    com_dc->restart();
}

CommercialDetector::CommercialDetector(const string& _cache, const string& closenessInfo, float _tolerance)
:cache(_cache), info_filename(closenessInfo), tolerance(_tolerance), video_mapper(), commercial_mapper(), last_video("")
{
}

CommercialDetector::~CommercialDetector()
{
    ifstream closenessInfo;
    if (closenessInfo.is_open())
        closenessInfo.close();
    if(result.is_open())
        result.close();

    detectionInfoList.clear();

    if (video_dc != nullptr)
        delete video_dc;

    if(voting_table != nullptr)
    {
        int videos_count = video_mapper.size();
        for(int i = 0; i < videos_count; i++)
            delete [] voting_table[i];
        delete [] voting_table;
    }
    video_mapper.clear();
    commercial_mapper.clear();
}

void CommercialDetector::init()
{
    video_mapper.clear();
    commercial_mapper.clear();

    vector<string> directorios = listar_directorios(cache);                             // se lista directorio de tv
    if (directorios.size() < 2)
        throw NewException("SE esperaban dos directorios uno de tv y otro de comerciales");

    string tvcache(directorios[0]);
    string comcache(directorios[1]);

    if(tvcache.find("television") == -1)
    {
        tvcache = directorios[1];
        comcache = directorios[0];
    }

    vector<string> tv = listar_directorios(tvcache);
    vector<string> comerciales = listar_directorios(comcache);

    voting_table = new int*[tv.size()];
    int tvcounter = 0, csize = comerciales.size();
    for (auto tvvid : tv)                         // se crean y almacenan todos los descriptores de video
    {
        string name(basename(tvvid));
        tvcounter = video_mapper.size();
        voting_table[tvcounter] = new int[csize];
        video_mapper.insert(make_pair(name, tvcounter++));
    }
    int comcounter = 0;
    for (auto comvid : comerciales)                // se crean y almacenan todos los descriptores de comerciales
    {
        string name(basename(comvid));
        comcounter = commercial_mapper.size();
        commercial_mapper.insert(make_pair(name, comcounter));
    }

    for (int i = 0; i < tvcounter; i++)
    {
        for (int j = 0; j < comcounter; j++)
        {
            voting_table[i][j] = 0;
        }
    }
}

void CommercialDetector::reset()
{
    int size = detectionInfoList.size();

    this->video_dc = new VideoDescriptorController(cache+"/television/"+last_video);

    if (size > 0)
    {
        for (auto& di: detectionInfoList)
        {
            di.reset();
        }
    }
    else
    {
        size = commercial_mapper.size();
        auto cursor = commercial_mapper.begin(), end = commercial_mapper.end();
        detectionInfoList.reserve(size);
        while(cursor != end)
        {
            detectionInfoList.push_back(DetectionInfo(cache+"/comerciales/"+cursor->first));
            ++cursor;
        }
    }
}

void CommercialDetector::execute()
{
    this->closenessInfo = ifstream(this->info_filename, ios::binary);
    if(!closenessInfo.is_open())
        throw NewException("Se esperaba poder abrir el fichero de resultados del proceso de busqueda de similitud");

    this->init();

    result = ofstream(this->cache+"/detecciones.txt", ios::out);
    int count = 0;
    string current_video;
    while(!closenessInfo.eof())
    {
        char _line[1024] = {'\0'};
        closenessInfo.read( (char *) &_line, sizeof(_line) );      // cargamos el nombre
        string line(_line);
        if(line.empty())
            continue;

        int filepos = line.find("##"), frame_no, comm_no, compos = line.rfind("##");
        int seppos = line.find("--");
        string video_name, commercial_name;
        if (filepos != -1 && seppos != -1)
        {
            video_name = line.substr(0, filepos);
            string no = line.substr(filepos+2, seppos-filepos-2);
            frame_no = parse_int(no);
        }

        if(compos != -1 && seppos != -1)
        {
            string no = line.substr(compos+2, line.size()-compos-2);
            commercial_name = line.substr(seppos+2, compos-seppos-2);

            comm_no = parse_int(no);
        }

        if(video_name != last_video)
        {
            last_video = video_name;
            this->reset();
        }

        computeVote(video_name, commercial_name, frame_no, comm_no);
    }

    closenessInfo.close();
    result.close();
}

void CommercialDetector::computeVote(const string& video_segment, const string& com_segment, long frame_no, long com_no)
{
    if(video_segment.empty() || com_segment.empty())
        return;
    int vpos = video_mapper[video_segment];
    int cpos = commercial_mapper[com_segment];

    DetectionInfo& di = detectionInfoList[cpos];

    const VideoMetadata& vmd = this->video_dc->getMetadata();
    const VideoMetadata& cvmd = di.com_dc->getMetadata();

    if(!di.isValid())
    {
        di.update(frame_no, 0);
    }

    long vid_diff = frame_no - di.current_frame;
    long com_diff = com_no - di.com_frame;
    long vdps = vmd.descriptors_per_second;
    long cdps = cvmd.descriptors_per_second+1;

    if( vid_diff/vmd.offset < vdps/**3 &&
        abs(com_diff)/cvmd.offset < cdps*/ )
    {
        if (this->voting_table[vpos][cpos] == 0)
        {
            di.video_init_frame = frame_no;
            di.com_init_frame = com_no;
        }

        ++this->voting_table[vpos][cpos];

        int totalDescriptors = (cvmd.frame_length/cvmd.offset);
        di.update(frame_no, com_no);
        if(this->voting_table[vpos][cpos] >= totalDescriptors*tolerance)
        {
            this->reportingResults(di, frame_no, com_no);

            this->voting_table[vpos][cpos] = 0;
            di.reset();
        }
    }
    else if (di.isValid())
    {
        this->voting_table[vpos][cpos] = 1;
//        di.reset();
        di.update(frame_no, com_no);
    }
}

float CommercialDetector::computeAccuracy(const DetectionInfo& di, long com_no)
{
    const VideoMetadata& vmd = this->video_dc->getMetadata();
    const VideoMetadata& cvmd = di.com_dc->getMetadata();
    float remaining_frames = (cvmd.frame_length - com_no);
    float video_size = cvmd.frame_length;

    if(video_size == 0)
        video_size = 1;

    float acc = (video_size - di.com_init_frame - remaining_frames) * 100 / video_size;
    if(acc < 45)
        acc = 100*tolerance-acc;
    if(acc > 100)
        acc = 99;
    return acc;
}

void CommercialDetector::reportingResults(const DetectionInfo& di, long frame_no, long com_no)
{
    const VideoMetadata& vmd = this->video_dc->getMetadata();
    const VideoMetadata& cvmd = di.com_dc->getMetadata();

    float dur = ((float)(frame_no - di.video_init_frame))/vmd.frame_per_second;

    cout<<endl<<basename(this->video_dc->getMetadata().name);                // tv show name
    cout<<"\t"<< ((float)di.video_init_frame)/vmd.frame_per_second;                                       // init time in seconds
    cout<<"\t"<< dur;               // time duration in seconds
    cout<<"\t"<< basename(cvmd.name);                                                    // commercial name
    cout<<"\t"<< this->computeAccuracy(di, com_no);                                          // accuracy

    this->result<<endl<<basename(this->video_dc->getMetadata().name);                // tv show name
    this->result<<"\t"<< ((float)di.video_init_frame)/vmd.frame_per_second;                            // init time in seconds
    this->result<<"\t"<< ((float)(frame_no - di.video_init_frame))/vmd.frame_per_second;               // time duration in seconds
    this->result<<"\t"<< basename(cvmd.name);                                                    // commercial name
    this->result<<"\t"<< this->computeAccuracy(di, com_no);                                          // accuracy
}
