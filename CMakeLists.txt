cmake_minimum_required(VERSION 3.13)
project(CommercialDetector)

find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 11)

add_executable(CommercialDetector main.cpp util.hpp util.cpp commercial_detector.cpp commercial_detector.h feature_extraction_controller.h feature_extraction_controller.cpp video_descriptor_controller.cpp video_descriptor_controller.h video_metadata.cpp video_metadata.h similarity_search_controller.cpp similarity_search_controller.h)

include_directories(CommercialDetector ${OpenCV_INCLUDE_DIRS})
target_link_libraries(CommercialDetector ${OpenCV_LIBS})