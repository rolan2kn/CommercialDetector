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

using namespace cv;
using namespace std;
int main( int argc, char** argv )
{
    string currentDIr(directorio_actual() + "data");
    string target_videos = "/television";
    string fragment_videos = "/comerciales";
    string tv(currentDIr + target_videos);
    string comm(currentDIr + fragment_videos);
    string cache(currentDIr + "/cache");

    string tv_desc(cache + target_videos);
    string com_desc(cache + fragment_videos);
    SimilaritySearchController ssc(tv_desc, com_desc, cache);
    ssc.fillData();
//    FeatureExtractionController fec(tv, comm, cache);
//    fec.execute();



//    if( argc > 1)
//    {
//        imageName = argv[1];
//    }
//    Mat image;
//    image = imread( imageName, -1 ); // Read the file
//    if( image.empty() )                      // Check for invalid input
//    {
//        cout <<  "Could not open or find the image" << std::endl ;
//        return -1;
//    }
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
    waitKey(0); // Wait for a keystroke in the window
    return 0;
}