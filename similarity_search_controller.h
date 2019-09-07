//
// Created by rkindela on 28-08-19.
//

#ifndef COMMERCIALDETECTOR_SIMILARITY_SEARCH_CONTROLLER_H
#define COMMERCIALDETECTOR_SIMILARITY_SEARCH_CONTROLLER_H
#include <iostream>
#include <vector>

#include "video_descriptor_controller.h"
using namespace std;

/**
 * SimilaritySearchController
 *
 * CLase que encapsula la logica de hallar la similitud entre descriptores y almacena
 * los resultados en un archivo binario
 *
 * Atributos
 * - string tv_path;
 * Representa la ruta de los descriptores de todos los videos de television seria algo como:
 * <CUALQUIER_COSA>/DIR_PROYECTO/data/cache/<target_videos>
 * normalmente <target_videos> = television
 *
 * - string comm_path;
 * Representa la ruta de los descriptores de todos los videos comerciales seria algo como:
 * <CUALQUIER_COSA>/DIR_PROYECTO/data/cache/<comerciales>
 * normalmente <comerciales> = comerciales
 *
 * - string cache;
 * Es la ruta donde se guardan todos los datos temporales. Normalmente es:
 * <CUALQUIER_COSA>/DIR_PROYECTO/data/cache/
 *
 * - vector<VideoDescriptorController*> video_list;
 * Lista de controladores de videos de television, contiene uno por cada video.
 *
 * - vector<VideoDescriptorController*> commercial_list;
 * Lista de controladores de videos de comerciales, contiene uno por cada comercial.
 *
 * - ofstream closenessInfo;
 * Es el archivo binario donde se almacena la informacion de cercania entre videos de television y comerciales
 * */
class SimilaritySearchController
{
private:
    string tv_path;
    string comm_path;
    string cache;
    vector<VideoDescriptorController*> video_list;
    vector<VideoDescriptorController*> commercial_list;

    ofstream closenessInfo;

public:
    SimilaritySearchController(const string& _tv_desc_path, const string& _commercial_desc_path, const string& _cache);
    ~SimilaritySearchController();
    void execute();
    void execute1();
    void fillData();
    void updateCommercials(const vector<int>& vdcs);
    void kClosestAccordingToEuclidean(int k, const VideoDescriptorController* target_descriptor, vector<int>& closest);
    void saveClosenessInformation(const VideoDescriptorController* target_descriptor, vector<int>& closest);
    void compareAllAndSave(int pos);
    string getClosenessInformationFileName();
};


#endif //COMMERCIALDETECTOR_SIMILARITY_SEARCH_CONTROLLER_H
