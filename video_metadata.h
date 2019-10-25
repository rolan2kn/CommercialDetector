//
// Created by rolo on 29/8/19.
//

#ifndef COMMERCIALDETECTOR_VIDEO_METADATA_H
#define COMMERCIALDETECTOR_VIDEO_METADATA_H

#include <iostream>
#include <fstream>
using namespace std;

/**
 * VideoMetadata
 *
 * es una estructura que encapsula la informacion comun a todos los descriptores de un video
 * y guarda los detalles para poder manipularlos facilmente
 *
 * Atributos:
 *  - string name
 *  es el directorio de la forma: <CUALQUIER_COSA>/DIR_PROYECTO/data/cache/<TIPO_VIDEO>/<NOMBRE_VIDEO>/
 *  como todos los metadatos tienen el mismo nombre solo se diferencian por esta ruta
 *
 *  -frame_length
 *  Tamaño en frame del video, esto determina el ultimo descriptor almacenado
 *
 *  -frame_per_second;
 *  Frecuencia de muestreo, cantidad de frames por segundo,
 *
 *  - descriptors_per_second;
 *  Cantidad de descriptores que se van a almacenar por un segundo, debe ser menor que frame_per_second
 *
 *  -offset;
 *  Desplazamiento entre cada descriptor dentro del mismo video
 * */
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
    string getPrevDescriptorName(int currentDescriptor);
    inline bool isValid(){return (!name.empty() && frame_length > 0 && frame_per_second > 0);}
    static string metadataFileName(){return "metadata";}
};


#endif //COMMERCIALDETECTOR_VIDEO_METADATA_H
