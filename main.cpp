#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "util.hpp"

#include "feature_extraction_controller.h"
#include "similarity_search_controller.h"
#include "commercial_detector.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    try
    {
        vector<std::string> args_param = get_args_vector(argc, argv);
        string currentDIr(directorio_actual() + "/data");
        string tv;
        string comm;
        string cache(currentDIr + "/cache");

        if (args_param.size() != 3)
        {
            throw NewException("Se esperaban los argumentos");
        }
        else
        {
            tv = args_param[1];
            comm = args_param[2];
        }

        string target_videos = "/television";
        string fragment_videos = "/comerciales";

        FeatureExtractionController fec(tv, comm, cache, target_videos, fragment_videos);
        fec.execute();
//
        string tv_desc(cache + target_videos);
        string com_desc(cache + fragment_videos);
        SimilaritySearchController ssc(tv_desc, com_desc, cache);
        ssc.fillData();
        ssc.execute();

        string closenessFile(cache + "/closeness");
        CommercialDetector cd(cache, closenessFile, 0.78f);
        cd.execute();

    }
    catch (NewException& ne)
    {
        cout<<endl<<ne.what();
        cout<<endl<<"###########################";
        cout<<endl<<"### CommercialDetector Usage";
        cout<<endl<<"Dependencias g++, cmake 3.13, OpenCV 4 ";
        cout<<endl<<"para ejecutar abrir la consola y escribir el ejecutable \nseguido del directorio de los videos de television";
        cout<<", y seguido del directorio de los Comerciales. \nNOTA: se puede poner directamente un nombre de archivo de television en rutas absolutas:\n";
        cout<<endl<<">> CommercialDetector <VIDEO_PATH or VIDEO_FILENAME> <COMMERCIAL_PATH>\n\n";
    }
    catch (std::exception& e)
    {
        cout<<endl<<e.what();
    }


    waitKey(0); // Wait for a keystroke in the window
    cv::destroyAllWindows();
    return 0;
}
