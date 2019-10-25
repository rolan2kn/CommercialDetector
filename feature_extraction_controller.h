//
// Created by rkindela on 28-08-19.
//

#ifndef COMMERCIALDETECTOR_FEATURE_EXTRACTION_CONTROLLER_H
#define COMMERCIALDETECTOR_FEATURE_EXTRACTION_CONTROLLER_H

#include "video_descriptor_controller.h"
#include <iostream>
using namespace std;

/**
 * FeatureExtractionController
 *
 * Clase que se encarga de crear los descriptores y almacenarlos en fichero
 *
 * Atributos:
 * -string tv_path;
 * Representa el directorio donde estan los videos de television a procesar
 *
 * -string commercial_path;
 * Representa el directorio donde estan los videos de comerciales a procesar
 *
 * -string cache;
 * Representa el directorio donde se almacenaran los descriptores
 *
 * -string target_name;
 * Representa el directorio dentro del cache que almacena los descriptores de video de tv.
 *
 * -string comm_name;
 * Representa el directorio dentro del cache que almacena los descriptores de video de comerciales.
 *
 * -int descriptors_by_second;
 * Representa la cantidad de descriptores que se almacenaran por cada segundo.
 * */
class FeatureExtractionController {
private:
    string tv_path;
    string commercial_path;
    string cache;
    string target_name;
    string comm_name;
    int descriptors_by_second;
private:
    void create_descriptors(const string& video_path, const string& outputdir = "");
public:
    FeatureExtractionController(const string& _tv_path, const string& _commercial_path, const string& _cache,
                                const string& target, const string& fragment, int descriptors_per_second = 3);
    ~FeatureExtractionController();
    void execute();
};


#endif //COMMERCIALDETECTOR_FEATURE_EXTRACTION_CONTROLLER_H
