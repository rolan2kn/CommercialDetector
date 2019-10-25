//
// Created by rolo on 29/8/19.
//

#include "video_descriptor_controller.h"



#include <opencv2/imgproc.hpp>
#include "util.hpp"

/**
 * VideoDescriptorController::VideoDescriptorController(const string& video_dir)
 *
 * constructor que inicializa el descriptor a partir del directorio del video correspondiente
 * esta pensado para posicionarse en el primero de los descriptores de dicho video
 * */
VideoDescriptorController::VideoDescriptorController(const string& video_dir)
:metadata(), current_descriptor(), descriptor_id(0), rows(DESC_ORDER), cols(DESC_ORDER)
{
    metadata.fromFile(video_dir + "/" + VideoMetadata::metadataFileName());         // obtenemos el metadata
    descriptor_id = -metadata.offset;                                               // garantizamos comenzar por el primer descriptor
    this->stepForward();                                                            // y lo cargamos
}

/**
 * VideoDescriptorController::VideoDescriptorController()
 *
 * constructor vacio
 * esta pensado para cargar un descriptor especifico desde un fichero
 * */
VideoDescriptorController::VideoDescriptorController()
        :metadata(), current_descriptor(), descriptor_id(0), rows(DESC_ORDER), cols(DESC_ORDER)
{
}

/**
 * VideoDescriptorController::VideoDescriptorController(const VideoMetadata& vmd, const cv::Mat& frame, int _desc_id)
 *
 * constructor a partir de un metadato, un frame de video y un identificador (comunmente el numero de frame)
 * esta pensado para crear el descriptor a partir del frame de video y descriptor_id y almacenarlo en un archivo luego
 * */
VideoDescriptorController::VideoDescriptorController(const VideoMetadata& vmd, const cv::Mat& frame, int _desc_id)
:metadata(vmd), current_descriptor(), descriptor_id(_desc_id), rows(DESC_ORDER), cols(DESC_ORDER)
{
    cv::resize(frame, current_descriptor, cv::Size(cols, rows));
}

VideoDescriptorController::~VideoDescriptorController()
{
    current_descriptor.release();
}

/**
 * VideoDescriptorController::getEuclideanDistance(const VideoDescriptorController& vdc)
 *
 * metodo que calcula la distancia euclidiana entre dos descriptores
 * se usa para hallar dicha distancia entre video de tv y comerciales
 * */
double VideoDescriptorController::getEuclideanDistance(const VideoDescriptorController& vdc) const
{
    if(vdc.current_descriptor.empty() || this->current_descriptor.empty())
        throw NewException("Se esperaban descriptores validos");

    double dist = cv::norm(this->current_descriptor, vdc.current_descriptor);

    return dist;
}

/**
 * VideoDescriptorController::getHammingDistance(const VideoDescriptorController& vdc)
 *
 * metodo que calcula la distancia de hamming (elemento a elemento, no binaria) entre dos descriptores
 * se usa para hallar dicha distancia entre video de tv y comerciales
 * */
double VideoDescriptorController::getHammingDistance(const VideoDescriptorController& vdc) const
{
    if(vdc.current_descriptor.empty() || this->current_descriptor.empty())
        throw NewException("Se esperaban descriptores validos");

    int sum = 0;
    for (int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            sum += (this->current_descriptor.at<float>(i, j) != vdc.current_descriptor.at<float>(i, j)) ? 1 : 0;
        }
    }

    return (double)sum;
}

/**
 * VideoDescriptorController::getManhattanDistance(const VideoDescriptorController& vdc)
 *
 * metodo que calcula la distancia manhattan o L1 entre dos descriptores
 * se usa para hallar dicha distancia entre video de tv y comerciales
 * */
double VideoDescriptorController::getManhattanDistance(const VideoDescriptorController& vdc) const
{
    if(vdc.current_descriptor.empty() || this->current_descriptor.empty())
        throw NewException("Se esperaban descriptores validos");

    int sum = 0;
    for (int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            sum += std::abs(this->current_descriptor.at<float>(i, j) - vdc.current_descriptor.at<float>(i, j));
        }
    }

    return (double)sum;
}

/**
 * VideoDescriptorController::getHammingDistance(const VideoDescriptorController& vdc)
 *
 * metodo que calcula la distancia de hamming (elemento a elemento, no binaria) entre dos descriptores
 * se usa para hallar dicha distancia entre video de tv y comerciales
 * */
double VideoDescriptorController::getDistance(const VideoDescriptorController& vdc) const
{
    return this->getEuclideanDistance(vdc);
}

/**
 * VideoDescriptorController::toFile(const string& filename)
 *
 * Metodo que permite serializar el descriptor a un fichero binario llamado filename
 * como existe el archivo metadata que describe el video solo necesitamos almacenar el descriptor concreto
 * */
void VideoDescriptorController::toFile(const string& filename)
{
    if(this->current_descriptor.empty())                            // se verifica que el descriptor exista
        return;
    
    ofstream descriptor_file(filename, ios::binary);                // se crea el archivo binario
    if (descriptor_file.is_open())                                  // si se logro abrir
    {
        rows = this->current_descriptor.rows;
        cols = this->current_descriptor.cols;       // capturamos filas y columnas
        size_t type = this->current_descriptor.type();

        descriptor_file.write( (char *) &rows, sizeof(int) );                                 // almacenamos filas
        descriptor_file.write( (char *) &cols, sizeof(int) );                                 // almacenamos columnas
        descriptor_file.write( (char *) &type, sizeof(type) );                                 // almacenamos el tipo de datos


        for(int i=0; i<rows;i++)
        {
            for(int j=0; j<cols; j++)
            {
                uchar pixelij = this->current_descriptor.at<uchar>(i, j);
                descriptor_file.write( (char *) &pixelij, sizeof(uchar) );           // y lo guardamos. De esa forma controlamos problemas de memoria
            }                                                                        // la memoria del cv::Mat
        }
    }

    descriptor_file.close();                                                        // cerramos el fichero
}

/**
 * VideoDescriptorController::fromFile(const string& filename)
 *
 * Metodo que permite deserializar el descriptor desde un fichero binario llamado filename
 *
 * como se requiere el archivo metadata que describe el video se comprueba que exista y sino se carga tambien
 * y se cargan del fichero especificado los datos del descriptor.
 *
 * Hay que hacerlo de este modo para que el objeto cv::Mat se cree apropiadamente y no resulte en desbordamientos de memoria
 * el nombre del fichero es un codigo como sigue:
 *
 * <CUALQUIER_COSA>/DIR_PROYECTO/data/cache/<TIPO_VIDEO>/<NOMBRE_VIDEO>/<NUMERO DE FRAME DEL DESCRIPTOR>
 *
 * donde <TIPO_VIDEO> puede ser: television o comerciales
 * <NUMERO DE FRAME DEL DESCRIPTOR> es el nombre del descriptor
 *
 * */
bool VideoDescriptorController::fromFile(const string& filename)
{
    if (filename.empty())                                               // se verifica nombre de fichero valido
        return false;

    string name = basename(filename);           // se obtiene el <NUMERO DE FRAME DEL DESCRIPTOR>
    if(!metadata.isValid())                     // si aun no existe el metadata lo cargamos
    {
        string path = filename.substr(0, filename.length()-name.length());    //obtengo el path del fichero sin el numero de frame
        metadata.fromFile(path + "/" + VideoMetadata::metadataFileName());            // añado el nombre del metadata y lo cargo
    }
    descriptor_id = parse_int(basename(filename));                         // se obtiene el id del descriptor o frame del video actual

    bool result = false;

    ifstream desc_file(filename, ios::binary);                      // se abre le fichero
    if (desc_file.is_open())
    {
        desc_file.read( (char *) &rows, sizeof(int) );              // leo filas y columnas
        desc_file.read( (char *) &cols, sizeof(int) );

        size_t type;
        desc_file.read( (char *) &type, sizeof(type) );                                 // almacenamos el tipo de datos

        this->current_descriptor = cv::Mat(rows, cols, type);    // creo un objeto cv::Mat apropiado
        for(int i=0; i<rows;i++)
        {
            for(int j=0; j<cols; j++)
            {
                uchar pixelij = '\0';
                desc_file.read( (char *) &pixelij, sizeof(uchar) );           // y lo guardamos. De esa forma controlamos
                this->current_descriptor.at<uchar>(i, j) = pixelij;         // de otra manera se crean problemas de memoria
            }
        }
        result = true;
    }
    desc_file.close();                                                      // cierro el fichero

    return result;                                                          // y digo como salio el proceso
}

/**
 * VideoDescriptorController::stepForward()
 *
 * metodo que permite actualizar el controlador con el descriptor correspondiente
 * al siguiente frame de video almacenado segun el framerate y el offset
 *
 * esto permite tener un solo objeto por video que controla
 * todos los descriptores derivados del mismo video
 *
 * retorna verdadero o falso si se logra realizar la operacion
 * */
bool VideoDescriptorController::stepForward()
{
    if (!this->anyDataRemains())        // Si procesamos el ultimo descriptor
        return false;                   // es imposible avanzar más y retornamos false

    string next_desc_name = metadata.getNextDescriptorName(descriptor_id);      // avanzamos al siguiente descriptor
    return this->fromFile(next_desc_name);                                      // y retornamos el resultado de cargarlo
}

/**
 * VideoDescriptorController::stepBackward()
 *
 * metodo que permite actualizar el controlador con el descriptor correspondiente
 * al anterior frame de video almacenado segun el framerate y el offset
 *
 * esto permite tener un solo objeto por video que controla
 * todos los descriptores derivados del mismo video
 *
 * retorna verdadero o falso si se logra realizar la operacion
 * */

bool VideoDescriptorController::stepBackward()
{
    if (this->descriptor_id == 0)        // Si procesamos el ultimo descriptor
        return false;                   // es imposible avanzar más y retornamos false

    string next_desc_name = metadata.getPrevDescriptorName(descriptor_id);      // avanzamos al anterior descriptor
    return this->fromFile(next_desc_name);
}

/**
 * VideoDescriptorController::restart()
 *
 * Metodo que permite regresar al comienzo de los descriptores
 * es decir al descriptor del frame 0 de video.
 *
 * es util para realizar el reconocimiento de los descriptores en otro video
 *
 * retorna verdadero o falso si se puede realizar la acción
 * */
bool VideoDescriptorController::restart()
{
    this->descriptor_id = -metadata.offset; // retrocedo para caer en el descriptor cero
    return this->stepForward();             // y devuelvo el resultado de cargarlo
}

/**
 * VideoDescriptorController::anyDataRemains()
 *
 * Metodo que permite determinar si no es posible continuar leyendo descriptores porque llegamos al final.
 *
 * es util para determinar el final de un video o el momento de volver a comenzar.
 *
 * retorna verdadero o falso si se puede realizar la acción
 * */
bool VideoDescriptorController::anyDataRemains()
{
    return (this->descriptor_id + metadata.offset < metadata.frame_length);     // verifica que se puede avanzar o no
}

/**
 * VideoDescriptorController::goTo(int position)
 *
 * Metodo que permite posicionarse en el descriptor especificado por pos
 *
 * es util para posicionarse en un descriptor especifico de un video.
 *
 * retorna verdadero o falso si se puede realizar la acción
 * */
bool VideoDescriptorController::goTo(int position)
{
    this->descriptor_id = position-metadata.offset; // asigno en la posicion menos el offset para avanzar a la posicion requerida
    return this->stepForward();             // y devuelvo el resultado de cargarlo
}
