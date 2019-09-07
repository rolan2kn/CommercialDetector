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

FeatureExtractionController::FeatureExtractionController(const string& _tv_path, const string& _commercial_path,
                                                        const string& _cache, const string& target, const string& fragment, int desc_ps)
: tv_path(_tv_path), commercial_path(_commercial_path), cache(_cache),
target_name(target), comm_name(fragment), descriptors_by_second(desc_ps)
{
    if (target_name.empty())
    {
        target_name = "television";
    }
    if(comm_name.empty())
    {
        comm_name = "comerciales";
    }
}

FeatureExtractionController::~FeatureExtractionController()
{

}

void FeatureExtractionController::execute()
{
    if (tv_path.empty())                                                                            // Chequeo integridad del directorio de tv
        throw NewException("Se espera un archivo o directorio de videos de televisión");

    if (commercial_path.empty())                                                                    // Chequeo integridad del directorio de comerciales
        throw NewException("Se espera un archivo o directorio de videos de comerciales");

    vector<string> tv_videos = listar_archivos(tv_path);                                            // obtengo lista de videos de tv
    vector<string> commercial_videos = listar_archivos(commercial_path);                            // obtengo lista de videos de comerciales

    if (tv_videos.empty())                                                                  // si es un video de tv y no un directorio
        tv_videos.push_back(tv_path);                                                       // agrego el video a la lista

    if(commercial_videos.empty())                                                           // si es un video de comercial y no un directorio
        commercial_videos.push_back(commercial_path);                                       // lo agrego a la lista

    if(cache.empty())                                                                           // si el cache no existe
    {
        cache = directorio_actual() + "/data/cache";                                       // lo creo en el directorio actual
    }

    for (auto tvvid : tv_videos)                                                            // por cada video de tv creo descriptores
    {
        this->create_descriptors(tvvid, cache + "/" + target_name);
    }

    for (auto comvid : commercial_videos)                                                   // por cada video de comerciales creo descriptores
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

        if(current_frame % offset != 0 && /*current_frame % video_fps != 0 && */current_frame != frame_length)            //Si no es el ultimo frame y tampoco es un frame correspondiente al downsampling
        {
            ++current_frame;                                                        // Salto el frame
            continue;
        }

        string name = video_desc_dir + "/" + std::to_string(current_frame);         // Genero el nombre del fichero del descriptor a partir del frame y el directorio del video/comercial

        if (existe_archivo(name) || !capture.retrieve(frame))                       // Si ya existe o no puedo acceder al frame actual
        {
            ++current_frame;                                                        // Salto el frame
            continue;
        }

        //convertir a gris
        cv::cvtColor(frame, frame_gris, cv::COLOR_BGR2GRAY);                  // Convierto a escala de grises

        mostrar_frame("Video", frame_gris, false, false);

        VideoDescriptorController* vdc = new VideoDescriptorController(vmd, frame_gris, current_frame);                 // Creo el descriptor
        vdc->toFile(name);                                                                                              // y lo guardo en el fichero "name"
        mostrar_frame("desc saved", vdc->getCurrentDescriptor(), false, false);

        VideoDescriptorController* rvdc = new VideoDescriptorController();                 // Creo el descriptor
        rvdc->fromFile(name);                                                                                              // y lo guardo en el fichero "name"
        mostrar_frame("desc loaded", rvdc->getCurrentDescriptor(), false, false);

        ++current_frame;                                                                                                // paso al siguiente frame

        char key = cv::waitKey(1) & 0xFF;
        if (key == ' ')
            key = cv::waitKey(0) & 0xFF;
        if (key == 'q' or key == 27)
            break;

        cout<<endl<<"PROCESSED: "<<name;
        cout.flush();
    }

    capture.release();                                  // Libero la captura
    cv::destroyAllWindows();                            // Borro todas las figuras, frames y matrices
}
