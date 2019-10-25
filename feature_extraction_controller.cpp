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



/**
 * FeatureExtractionController(const string& _tv_path, const string& _commercial_path,
 *                                                        const string& _cache, const string& target, const string& fragment, int desc_ps)
 *
 * Contructor
 *
 * */
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

/***
 * FeatureExtractionController::~FeatureExtractionController()
 *
 * Destructor
 */
FeatureExtractionController::~FeatureExtractionController()
{

}

/**
 *  FeatureExtractionController::execute()
 *
 *  Metodo que prepara las condiciones para crear los descriptores y almacenarlos.
 * */
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

/**
 * FeatureExtractionController::create_descriptors(const string& video_path, const string& outputdir)
 *
 * Metodo que crea los descriptores de un video, los almacena en un fichero con el siguiente formato:
 *
 * <CUALQUIER_COSA>/DIR_PROYECTO/data/cache/<TIPO_VIDEO>/<NOMBRE_VIDEO>/
 *
 * donde outputdir = cache + "/" + target_name
 *
 * y el nombre del fichero completo: outputdir + /<NOMBRE REAL DEL VIDEO>/<NUMERO DE FRAME>
 *
 * se utiliza el numero de frame para saber en todo momento dicha informacion.
 *
 * Tambien se crea un archivo de metadatos y se almacena en outputdir + /<NOMBRE REAL DEL VIDEO>/metadatos
 *
 * Ver estructura VideoMetadata
 *
 * */
void FeatureExtractionController::create_descriptors(const string& video_path, const string& outputdir)
{
    if(!existe_archivo(video_path))
        throw NewException("EL archivo de video no existe, quizas el directorio este erroneo");
    string video_desc_dir(outputdir + "/" + basename(video_path));             // se crea el directorio correspondiente al video
    crear_directorio(video_desc_dir);

    cv::VideoCapture capture = abrir_video(video_path);                        // se abre el video

    cv::Mat frame, frame_gris;
    int current_frame = 0, video_fps = capture.get(cv::CAP_PROP_FPS);   // se obtienen algunos metadatos

    if(this->descriptors_by_second > video_fps)                                // y se configuran los descriptores por segundo
        this->descriptors_by_second = 3;
    else if(this->descriptors_by_second == 0)
        this->descriptors_by_second = 3;

    int offset = video_fps/(this->descriptors_by_second);                       // se define el offset en funcion de los descriptores por segundo
    int frame_length = capture.get(cv::CAP_PROP_FRAME_COUNT);          // y se obtiene la cantidad de frames total del video

    VideoMetadata vmd(video_desc_dir, frame_length, video_fps, this->descriptors_by_second);        // se definen los metadatos
    vmd.toFile(video_desc_dir+"/"+VideoMetadata::metadataFileName());                               // y se almacenan

    while (capture.grab()) {

        if(current_frame % offset != 0)            //Si no es el ultimo frame y tampoco es un frame correspondiente al downsampling
        {
            ++current_frame;                                                        // entonces no me interesa y lo salto
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

         VideoDescriptorController* vdc = new VideoDescriptorController(vmd, frame_gris, current_frame);                 // Creo el descriptor
         vdc->toFile(name);                                                                                                              // y lo guardo en el fichero "name"

        ++current_frame;

        cout<<endl<<"DESCRIPTOR PROCESSED: "<<name;
        cout.flush();
    }

    capture.release();                                  // Libero la captura
    cv::destroyAllWindows();                            // Borro todas las figuras, frames y matrices
}
