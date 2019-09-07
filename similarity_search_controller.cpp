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
    vector<string> tv_videos = listar_directorios(tv_path);                             // se lista directorio de tv
    vector<string> commercial_videos = listar_directorios(comm_path);                   // se lista directorio de comerciales

    if(cache.empty())                                                                  // se verifica el cache
    {
        cache = directorio_actual() + "/data/cache";
    }

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

    string filename(this->getClosenessInformationFileName() + ".txt");
    closenessInfo = ofstream(filename, ios::out);

    if(!closenessInfo.is_open())
        throw NewException("Se esperaba el fichero listo para editar");

//    closenessInfo = ofstream(filename, ios::binary);


    int k = 3;                                              // se configura el numero de similitudes a considerar
    int size = video_list.size();
    closenessInfo<<size;
    cout<<endl<<size;
//    closenessInfo.write((char*)&size, sizeof(int));         // se guarda el numero de videos de tv
    int previous_vdc, current_vdc;

    for(int i = 0; i < size; i++)         // por cada descriptor de video de tv
    {
        this->compareAllAndSave(i);
    }
    closenessInfo.close();
}

void SimilaritySearchController::execute1()
{
    if(closenessInfo.is_open())         // si el archivo estaba abierto
        closenessInfo.close();          // se cierra

    string filename(this->getClosenessInformationFileName() + ".txt");
    closenessInfo = ofstream(filename, ios::out);

    if(!closenessInfo.is_open())
        throw NewException("Se esperaba el fichero listo para editar");

//    closenessInfo = ofstream(filename, ios::binary);


    int k = 3;                                              // se configura el numero de similitudes a considerar
    int size = video_list.size();
    closenessInfo<<size;
    cout<<endl<<size;
//    closenessInfo.write((char*)&size, sizeof(int));         // se guarda el numero de videos de tv
    int previous_vdc, current_vdc;

    for(VideoDescriptorController* vdc: video_list)         // por cada descriptor de video de tv
    {
//        char tvname[1024] = {'\0'};                                                         // creamos un arreglo temporal
//        strncpy(tvname, vdc->getMetadata().name.c_str(), vdc->getMetadata().name.size());    // y copiamos el nombre
//        closenessInfo.write((char*)&tvname, sizeof(tvname));                                    // para almacenarlo en fichero
        closenessInfo<<endl<<vdc->getMetadata().name;                                    // para almacenarlo en fichero
        cout<<endl<<vdc->getMetadata().name;                                    // para almacenarlo en fichero

        while(vdc->anyDataRemains())                                         // mientras queden descriptores del video
        {
            vector<int> closest;
            this->kClosestAccordingToEuclidean(3, vdc, closest);      // obtenemos los comerciales mas parecidos
            this->saveClosenessInformation(vdc, closest);                // y los guardamos a fichero
            this->updateCommercials(closest);                               // y reiniciamos o avanzamos los descriptores de comerciales
            vdc->stepForward();                                             // avanzamos tambien el video
        }
    }
    closenessInfo.close();
}

/**
 * SimilaritySearchController::updateCommercials
 *
 * Metodo que actualiza los descriptores de comerciales a partir de una lista de comerciales similares a un video
 *
 * si un descriptor de comercial esta en la lista de similares entonces se avanza al siguiente descriptor
 * sino entonces se reinicia al inicio, asi al avanzar el frame se chequea con el primer descriptor de comercial
 *
 * */
void SimilaritySearchController::updateCommercials(const vector<int>& vdcs)
{
    int size = commercial_list.size();
    int list_size = vdcs.size();
    for(int i = 0; i < size; i++)
    {
        int p = 0;
        bool found = false;
        while(p < list_size && !found)      // chequeamos que el descriptor actual de comercial es de los similares
        {
            found = (vdcs[p] == i);
            ++p;
        }
        VideoDescriptorController* vdc = commercial_list[i];
        if(!found)                                              // si no es similar
            vdc->restart();                                     // lo reiniciamos
        else                                                    // pero si es similar
        {
            if(!vdc->stepForward())                             // entonces avanzamos al siguiente, y si no se puede,
                vdc->restart();                                 // entonces lo reiniciamos porque llego al final
        }
    }
}

/**
 * SimilaritySearchController::kClosestAccordingToEuclidean(int k, const VideoDescriptorController* target_descriptor, vector<int>& closest)
 *
 * Metodo que obtiene la lista de indices de los k descriptores de comerciales mas similares a un descriptor de video de tv.
 *
 * */
void SimilaritySearchController::kClosestAccordingToEuclidean(int k, const VideoDescriptorController* target_descriptor, vector<int>& closest)
{
    int size = commercial_list.size();
    int umbral = 400;
    map<double, int> distanceMapper;            // creo un mapa o tabla hash que mantiene ordenado los keys,
                                                // se indexa la distancia con el indice
    for(int i = 0; i < size; i++)               // por cada i
    {
        VideoDescriptorController* vdc = commercial_list[i];
        double euclideanDistance = target_descriptor->getEuclideanDistance(*vdc);   // obtengo la distancia del descritor de video y comercial

        while (!distanceMapper.insert(std::make_pair(euclideanDistance, i)).second)     //almaceno la distancia, sino se puede
        {
            euclideanDistance += 0.05;                                                  // incremento decimales hasta que se pueda
        }
    }
    int i = 0;                                      // inicializo el contador de similares
    for(auto pair: distanceMapper)                  // como el mapa esta ordenado solo necesito iterar
    {
        if (i < k)                                  // y mientras no tenga los k similares
        {
            closest.push_back(pair.second);         // los voy almacenando
            ++i;
        }
    }
    distanceMapper.clear();                         // borro los datos del mapa.
}

void SimilaritySearchController::compareAllAndSave(int pos)
{
    int size = commercial_list.size();
    int umbral = 400;

    VideoDescriptorController* target_descriptor = video_list[pos];
    closenessInfo<<endl<<target_descriptor->getMetadata().name;                                    // guardo el nombre del fichero
    cout<<endl<<vdc->getMetadata().name;                                    // para almacenarlo en fichero
    while (target_descriptor->anyDataRemains())
    {
        closenessInfo<<endl<<target_descriptor->getDescriptorId();                                    // guardo el id del video
        cout<<endl<<target_descriptor->getDescriptorId();                                    // para almacenarlo en fichero

        for(int i = 0; i < size; i++)               // por cada i
        {
            VideoDescriptorController* vdc = commercial_list[i];
            vdc->restart();

            closenessInfo<<endl<<"###########################";
            closenessInfo<<endl<<vdc->getMetadata().name;                                    // guardo el nombre del comercial
            closenessInfo<<endl<<vdc->getMetadata().frame_length;                                    // guardo el tamaño del comercial
            closenessInfo<<endl<<vdc->getMetadata().offset;                                    // y el offset


            cout<<endl<<vdc->getMetadata().frame_length;                                    // guardo el tamaño del comercial
            cout<<endl<<vdc->getMetadata().offset;                                    // y el offset

            while(vdc->anyDataRemains())
            {
                double euclideanDistance = target_descriptor->getEuclideanDistance(*vdc);   // obtengo la distancia del descritor de video y comercial
                closenessInfo<<endl<<vdc->getDescriptorId();                                    // y el frame actual del descriptor
                closenessInfo<<endl<<euclideanDistance;                                    // para almacenarlos en fichero

                vdc->stepForward();
            }
        }
        target_descriptor->stepForward();
    }


}

/**
 * SimilaritySearchController::saveClosenessInformation(const VideoDescriptorController* target_descriptor, vector<int>& closest)
 *
 * Metodo que almacena la informacion de similitud en el fichero binario closenessInfo
 * */
void SimilaritySearchController::saveClosenessInformation(const VideoDescriptorController* target_descriptor, vector<int>& closest)
{
    if (target_descriptor == nullptr)                                               // se verifica el descriptor de video
        throw NewException("Se esperaba un controlador de descriptor de video de televisión");
    if (!this->closenessInfo.is_open())                                             // se revisa que el fichero este listo
        throw NewException(" Se esperaba el fichero listo para ser editado");
    if(closest.empty())                                                             // y tambien que existan elementos similares
        return;

    int current_id = target_descriptor->getDescriptorId();
    this->closenessInfo<<endl<<current_id;     // se almacena el identificador (numero de frame)
    cout<<endl<<current_id;     // se almacena el identificador (numero de frame)
//    this->closenessInfo.write( (char *) &current_id, sizeof(int) );     // se almacena el identificador (numero de frame)
                                                                        // del video de tv

    int k = closest.size();
    this->closenessInfo<<endl<<k;
    cout<<endl<<k;
//    this->closenessInfo.write( (char *) &k, sizeof(int) );              // se almacenan la cantidad de descriptores similares
    for(int vdc_idx: closest)                                           // y por cada uno de ellos
    {
        VideoDescriptorController* vdc = this->commercial_list[vdc_idx];

        char desc_name[1024] = {'\0'};                                                      // se crea un nuevo arreglo para
        strncpy(desc_name, vdc->getMetadata().name.c_str(), vdc->getMetadata().name.size());// controlar la memoria

        int _current_id = vdc->getDescriptorId();
        int distance = (int) target_descriptor->getEuclideanDistance(*vdc);             // se obtiene el descriptor actual

//        this->closenessInfo.write( (char *) &desc_name, sizeof(string) );               // se guarda todo en fichero
//        this->closenessInfo.write( (char *) &_current_id, sizeof(int) );
//        this->closenessInfo.write( (char *) &distance, sizeof(int) );
        this->closenessInfo<<endl<<desc_name<<" "<<_current_id<<" "<<distance;
        cout<<endl<<desc_name<<" "<<_current_id<<" "<<distance;
    }
}

string SimilaritySearchController::getClosenessInformationFileName()
{
    string closenessFile(this->cache + "/closeness");
    return closenessFile;
}