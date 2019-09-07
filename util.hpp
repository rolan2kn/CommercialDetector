#pragma once

#include <opencv2/videoio.hpp>
#include <string>
#include <vector>
#include <exception>



std::vector<std::string> get_args_vector(int argc, char** argv);

int parse_int(const std::string &string);

double parse_double(const std::string &string);

bool existe_archivo(const std::string &filename);

std::string basename(const std::string &filename);

void mostrar_frame(const std::string &window_name, const cv::Mat &imagen, bool valorAbsoluto, bool escalarMin0Max255);

cv::VideoCapture abrir_video(const std::string &filename);

std::vector<std::string> leer_lineas_archivo(const std::string &filename);

std::vector<std::string> listar_archivos(const std::string &dirname);

std::vector<std::string> listar_directorios(const std::string &dirname);

std::string directorio_actual();

bool crear_directorio(std::string& dir);

bool existe_directorio(const std::string &dir);

class NewException: public std::exception
{
private:
    std::string error_msg;
public:
    NewException(const std::string& mesg): error_msg(mesg) {}
    const std::string& what() {return error_msg;}
};



