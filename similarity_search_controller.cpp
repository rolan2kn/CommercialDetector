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

/**
 * SimilaritySearchController::SimilaritySearchController(const string& _tv_desc_path, const string& _commercial_desc_path, const string& _cache)
 *
 * constructor
 * */
SimilaritySearchController::SimilaritySearchController(const string& _tv_desc_path, const string& _commercial_desc_path, const string& _cache)
:tv_path(_tv_desc_path), comm_path(_commercial_desc_path), cache(_cache), video_list(), commercial_list()
{

}

/**
 * SimilaritySearchController::fillData()
 *
 * MEtodo encargado de cargar los descriptores de videos de television y comerciales
 * y llenar las listas de cada uno
 * */
void SimilaritySearchController::fillData()
{
    if(cache.empty())                                                                  // se verifica el cache
    {
        cache = directorio_actual() + "/data/cache";
    }
    if(!existe_directorio(this->cache))
        throw NewException("No se encontraron los descriptores a procesar");

    vector<string> tv_videos = listar_directorios(tv_path);                             // se lista directorio de tv
    vector<string> commercial_videos = listar_directorios(comm_path);                   // se lista directorio de comerciales

    for (auto tvvid : tv_videos)                         // se crean y almacenan todos los descriptores de video
    {
        VideoDescriptorController* vdc = new VideoDescriptorController(tvvid);
        this->video_list.push_back(vdc);
    }
    for (auto comvid : commercial_videos)                // se crean y almacenan todos los descriptores de comerciales
    {
        VideoDescriptorController* vdc = new VideoDescriptorController(comvid);
        this->commercial_list.push_back(vdc);
    }
}

/**
 * SimilaritySearchController::~SimilaritySearchController()
 *
 * destructor
 * */
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
    commercial_list.clear();
    if(closenessInfo.is_open())
        closenessInfo.close();
}

/**
 * SimilaritySearchController::execute()
 *
 * MEtodo que calcula las similitudes entre videos y comerciales
 * y las guarda en el archivo binario closenessInfo
 * */
void SimilaritySearchController::execute()
{
    if(closenessInfo.is_open())         // si el archivo estaba abierto
        closenessInfo.close();          // se cierra

    string filename(this->getClosenessInformationFileName());
    closenessInfo = ofstream(filename);
//    closenessInfo = ofstream(filename, ios::binary);

    if(!closenessInfo.is_open())
        throw NewException("Se esperaba el fichero listo para editar");

    int k = 1;                                              // se configura el numero de similitudes a considerar

    for(VideoDescriptorController* vdc: video_list)         // por cada descriptor de video de tv
    {
        while(vdc->anyDataRemains())                                        // mientras queden descriptores del video
        {
            cout<<endl<<"PROCESSED FRAME_NO: "<<vdc->getDescriptorId();
            this->detectKClosestCommercialFrames(k, vdc);                   // busco los comerciales mas parecidos y los almaceno
            vdc->stepForward();                                             // avanzamos tambien el video
        }
    }
    closenessInfo.close();
}

/**
 * SimilaritySearchController::detectKClosestCommercialFrames(int k, const VideoDescriptorController* target_descriptor)
 *
 * Metodo que calcula los k frames de comerciales mas cercanos a video target
 *
 * */
void SimilaritySearchController::detectKClosestCommercialFrames(int k, const VideoDescriptorController* target_descriptor)
{
    int size = commercial_list.size();
    map<double, string> distanceMapper;            // creo un mapa o tabla hash que mantiene ordenado los keys,
                                                // se indexa la distancia con el indice
    double currentDistance;
    for(int i = 0; i < size; i++)               // por cada i
    {
        VideoDescriptorController* vdc = commercial_list[i];

        double currentDistance;
        vdc->restart();
        while(vdc->anyDataRemains())
        {
            int id = vdc->getDescriptorId();
            string segment(basename(vdc->getMetadata().name) + "##" + std::to_string(id));
            currentDistance = target_descriptor->getDistance(*vdc);

            while (!distanceMapper.insert(std::make_pair(currentDistance, segment)).second)       // almaceno la distancia, sino se puede
            {
                currentDistance += 0.05;                                                                  // incremento decimales hasta que se pueda
            }
            vdc->stepForward();
        }
    }
    auto cursor = distanceMapper.begin(), end = distanceMapper.end();
    int i = 0;                                      // inicializo el contador de similares
    while(cursor != end && i < k)                      // como el mapa esta ordenado solo necesito iterar k veces
    {
        string info = (*cursor).second;                             //obtengo la informacion
        this->saveClosenessInformation(target_descriptor, info);    //y la guardo a fichero

        ++i;
        ++cursor;
    }
    distanceMapper.clear();                         // borro los datos del mapa.
}

/**
 * SimilaritySearchController::saveClosenessInformation(const VideoDescriptorController* target_descriptor, vector<int>& closest)
 *
 * Metodo que almacena la informacion de similitud en el fichero binario closenessInfo
 * */
void SimilaritySearchController::saveClosenessInformation(const VideoDescriptorController* target_descriptor, const string& info)
{
    if (target_descriptor == nullptr)                                               // se verifica el descriptor de video
        throw NewException("Se esperaba un controlador de descriptor de video de televisión");
    if (!this->closenessInfo.is_open())                                             // se revisa que el fichero este listo
        throw NewException(" Se esperaba el fichero listo para ser editado");

    int current_id = target_descriptor->getDescriptorId();

    string target_info(basename(target_descriptor->getMetadata().name)+"##"+std::to_string(current_id));
    string connection(target_info+"--"+info);

    char _data[1024] = {'\0'};                      // se crea un arreglo de char alternativo para almacenar el nombre
    strncpy(_data, connection.c_str(), connection.size());           // y evitar problemas de memoria con el string
    this->closenessInfo.write( (char *) &_data, sizeof(_data) );                // se guarda el nombre
}

string SimilaritySearchController::getClosenessInformationFileName()
{

    string closenessFile(this->cache + "/closeness");
    return closenessFile;
}