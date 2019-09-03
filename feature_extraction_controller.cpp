//
// Created by rkindela on 28-08-19.
//

#include "feature_extraction_controller.h"
#include <fstream>
#include "util.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "video_metadata.h"
#include "video_descriptor_controller.h"

FeatureExtractionController::FeatureExtractionController(const string& _tv_path, const string& _commercial_path, const string& _cache, int desc_ps)
:tv_path(_tv_path), commercial_path(_commercial_path), cache(_cache), descriptors_by_second(desc_ps)
{
}

FeatureExtractionController::~FeatureExtractionController()
{

}

void FeatureExtractionController::execute()
{
    vector<string> tv_videos = listar_archivos(tv_path);
    vector<string> commercial_videos = listar_archivos(commercial_path);
    string tv_name = basename(tv_path);
    string comm_name = basename(commercial_path);

    if(cache.empty())
    {
        cache = directorio_actual();
    }

    for (auto tvvid : tv_videos)
    {
        this->create_descriptors(tvvid, cache + "/" + tv_name);
    }
    for (auto comvid : commercial_videos)
    {
        this->create_descriptors(comvid, cache + "/" + comm_name);
    }
}

void FeatureExtractionController::create_descriptors(const string& video_path, const string& outputdir)
{
    /**
     * Para crear los descriptores de un video hay que hacer lo siguiente:
     *
     * 1. generar los nombres de fichero para almacenar los descriptores
     * 1.1 sacar el nombre del video de video_path
     * 1.2 crear el directorio con el nombre del video video_desc_dir = outputdir/video_name/
     *
     * 2. crear cada descriptor y almacenarlo en el video_desc_dir
     * 2.1 abrir el video
     * 2.2 pedir frame i*freq_rate; freq_rate = video_freq_pseg / 3
     * 2.3 llamar el metodo create_descriptor
     * 2.4 almacenar el descriptor en dec_dir y poner i como sufijo al nombre del fichero
     * */
    string video_desc_dir(outputdir + "/" + basename(video_path));
    crear_directorio(video_desc_dir);

    cv::VideoCapture capture = abrir_video(video_path);

    cv::Mat frame, frame_gris, output_frame, output_frame_gris;
    int current_frame = 0, video_fps = capture.get(cv::CAP_PROP_FPS);

    if(this->descriptors_by_second > video_fps)
        this->descriptors_by_second = video_fps;
    else if(this->descriptors_by_second == 0)
        this->descriptors_by_second = 1;

    int offset = video_fps/this->descriptors_by_second;
    int frame_length = capture.get(cv::CAP_PROP_FRAME_COUNT);

    VideoMetadata vmd(video_desc_dir, frame_length, video_fps, this->descriptors_by_second);
    vmd.toFile(video_desc_dir+"/"+VideoMetadata::metadataFileName());

    while (capture.grab()) {

        if(current_frame % offset != 0 && current_frame != frame_length)
        {
            ++current_frame;
            continue;
        }

        string name = video_desc_dir + "/" + std::to_string(current_frame);

        if (existe_archivo(name) || !capture.retrieve(frame))
        {
            current_frame += offset;
            capture.set(cv::CAP_PROP_POS_FRAMES, current_frame);
            continue;
        }

        //convertir a gris
        cv::cvtColor(frame, frame_gris, cv::COLOR_BGR2GRAY);

        VideoDescriptorController* vdc = new VideoDescriptorController(vmd, frame, current_frame);
        vdc->toFile(name);
        ++current_frame;
        cout<<endl<<"PROCESSED: "<<name;
        cout.flush();
    }

    capture.release();
    cv::destroyAllWindows();
}
