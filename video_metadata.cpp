//
// Created by rolo on 29/8/19.
//

#include "video_metadata.h"
#include "util.hpp"

/**
 * VideoMetadata::VideoMetadata(const string& _name, int _frame_length, int _fps, int dps)
 *
 * constructor que crea los metadatos a partir de todos sus componentes
 *
 * se utiliza al construir los descriptores
 * */
VideoMetadata::VideoMetadata(const string& _name, int _frame_length, int _fps, int dps)
:name(_name), frame_length(_frame_length), frame_per_second(_fps),descriptors_per_second(dps)
{
    descriptors_per_second = (descriptors_per_second != 0) ? descriptors_per_second : 1;
    this->offset = frame_per_second/descriptors_per_second;
}

/**
 * VideoMetadata::VideoMetadata()
 *
 * Constructor vacio
 * utilizado para cargar los metadatos de determinado video desde algun fichero.
 * */
VideoMetadata::VideoMetadata()
:name(""), frame_length(0), frame_per_second(25),descriptors_per_second(3)
{

}

/**
 * VideoMetadata::toFile(const string& filename)
 *
 * Metodo para serializar metadatos a fichero binario
 * la estructura del nombre del fichero es la siguiente:
 *
 * <CUALQUIER_COSA>/DIR_PROYECTO/data/cache/<TIPO_VIDEO>/<NOMBRE_VIDEO>/metadata
 * */
void VideoMetadata::toFile(const string& filename)
{
    if (!this->isValid())                               // se verifica que el nombre de fichero sea valido
        return;

    ofstream output_file(filename, ios::binary);

    if(output_file.is_open())
    {
        char _name[1024] = {'\0'};                      // se crea un arreglo de char alternativo para almacenar el nombre
        strncpy(_name, name.c_str(), name.size());           // y evitar problemas de memoria con el string
        output_file.write( (char *) &_name, sizeof(_name) );                // se guarda el nombre
        output_file.write( (char *) &frame_length, sizeof(int) );           // el tamaño
        output_file.write( (char *) &frame_per_second, sizeof(int) );       // etc
        output_file.write( (char *) &descriptors_per_second, sizeof(int) );
    }
    output_file.close();
}

/**
 * VideoMetadata::fromFile(const string& filename)
 *
 * Metodo para deserializar metadatos desde fichero binario
 * la estructura del nombre del fichero es la siguiente:
 *
 * <CUALQUIER_COSA>/DIR_PROYECTO/data/cache/<TIPO_VIDEO>/<NOMBRE_VIDEO>/metadata
 * */
void VideoMetadata::fromFile(const string& filename)
{
    ifstream input_file(filename, ios::binary);

    if(input_file.is_open())
    {
        char _name[1024] = {'\0'};
        input_file.read( (char *) &_name, sizeof(_name) );      // cargamos el nombre
        this->name = string(_name);
        input_file.read( (char *) &frame_length, sizeof(int) );     // y todo lo demas
        input_file.read( (char *) &frame_per_second, sizeof(int) );
        input_file.read( (char *) &descriptors_per_second, sizeof(int) );
        this->offset = frame_per_second/descriptors_per_second;
    }
    input_file.close();
}
/**
 * VideoMetadata::getNextDescriptorName(int currentDescriptor)
 *
 * Metodo que retorna el nombre del proximo descriptor a partir del parametro
 *
 * si no es posible obtenerlo se retorna el string vacio
 * */
string VideoMetadata::getNextDescriptorName(int currentDescriptor)
{
    int next = currentDescriptor + offset;              // se calcula el proximo descriptor

    if (next > frame_length)                            // si sobrepasa el ultimo
    next = frame_length;                                // se acota

    string filename(this->name + "/" + std::to_string(next));       // se crea el nombre de fichero
    if(existe_archivo(filename))                                    // si existe el descriptor buscado
        return filename;                                            // se retorna
    return string();                                                // si no existe se retorna cadena vacia
}
