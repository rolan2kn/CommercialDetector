#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "util.hpp"

#include "feature_extraction_controller.h"
#include "similarity_search_controller.h"

//#define TV_VIDEO_PATH  "/home/rkindela/Documentos/RAMOS/Recuperacion de Informacion Multimedia/tarea_1/television/mega-2014_04_10.mp4"
//#define TV_VIDEO_PATH  "/home/rkindela/Documentos/RAMOS/Recuperacion de Informacion Multimedia/tarea_1/television/"

#define TV_VIDEO_PATH  "/mnt/D/RAMOS/Recuperacion de Informacion Multimedia/tarea_1/television/mega-2014_04_10.mp4"
//#define TV_VIDEO_PATH  "/mnt/D/RAMOS/Recuperacion de Informacion Multimedia/tarea_1/television/"

#define COMERCIALES_PATH "/mnt/D/RAMOS/Recuperacion de Informacion Multimedia/tarea_1/comerciales/"
//#define COMERCIALES_PATH "/home/rkindela/Documentos/RAMOS/Recuperacion de Informacion Multimedia/tarea_1/comerciales/"

using namespace cv;
using namespace std;
int main( int argc, char** argv )
{
    try
    {
        vector<std::string> args_param = get_args_vector(argc, argv);
        string currentDIr(directorio_actual() + "data");
        string tv;
        string comm;
        string cache(currentDIr + "/cache");

        if (args_param.size() < 3)
        {
            tv = TV_VIDEO_PATH;
            comm = COMERCIALES_PATH;
        }
        else
        {
            tv = args_param[1];
            comm = args_param[2];
        }

        string target_videos = "/television";
        string fragment_videos = "/comerciales";

//        FeatureExtractionController fec(tv, comm, cache, target_videos, fragment_videos);
//        fec.execute();

        string tv_desc(cache + target_videos);
        string com_desc(cache + fragment_videos);
        SimilaritySearchController ssc(tv_desc, com_desc, cache);
        ssc.fillData();
        ssc.execute();

//    Mat new_mat;
//    cv::resize(image, new_mat, cv::Size(10, 10));
//
//    string name = cache + "/" + std::to_string(1);
//    ofstream descriptor_file(name, ios::binary);
//    descriptor_file.write( (char *) &new_mat, sizeof(cv::Mat ) );
//    descriptor_file.close();
//    cv::Mat recover_img;
//    ifstream descriptor_f(name, ios::binary);
//    descriptor_f.read((char *) &recover_img, sizeof(cv::Mat ) );
//    descriptor_f.close();
//
//    namedWindow( "Display window", WINDOW_AUTOSIZE ); // Create a window for display.
//    namedWindow( "Display resized window", WINDOW_AUTOSIZE ); // Create a window for display.
//    imshow( "Display descriptor", new_mat );                // Show our image inside it.
//    imshow( "Display recovered descriptor", recover_img );                // Show our image inside it.
//    imshow( "Display window", image );                // Show our image inside it.
    }
    catch (exception& e)
    {
        cout<<endl<<e.what();
    }

    waitKey(0); // Wait for a keystroke in the window
    return 0;
}