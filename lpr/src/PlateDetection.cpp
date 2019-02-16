#include "../include/PlateDetection.h"
#include "util.h"

#include <opencv2/opencv.hpp>

namespace pr{
    PlateDetection::PlateDetection(std::string filename_cascade){
        cascade.load(filename_cascade);

    };
    void PlateDetection::plateDetectionRough(cv::Mat InputImage,std::vector<pr::PlateInfo>  &plateInfos,int min_w,int max_w){
        cv::Mat processImage;
        int h = InputImage.rows / 2;
        cv::Mat InputImage2 = InputImage(cv::Rect(0, h-1, InputImage.cols, h));

        cv::cvtColor(InputImage2,processImage,cv::COLOR_BGR2GRAY);
        std::vector<cv::Rect> platesRegions;
        cv::Size minSize(min_w,min_w/4);
        cv::Size maxSize(max_w,max_w/4);

        double timeStart = (double)cv::getTickCount();

        cascade.detectMultiScale( processImage, platesRegions,
                                  1.1, 3, cv::CASCADE_SCALE_IMAGE,minSize,maxSize);

        double DetectionTime = ((double)cv::getTickCount() - timeStart) / cv::getTickFrequency()*1000;
        std::cout<<"detectMultiScale time: "<<DetectionTime<<" Ms"<<std::endl;

        for(auto plate:platesRegions)
        {
            int zeroadd_w  = static_cast<int>(plate.width*0.30);
            int zeroadd_h = static_cast<int>(plate.height*2);
            int zeroadd_x = static_cast<int>(plate.width*0.15);
            int zeroadd_y = static_cast<int>(plate.height*1);
            plate.x-=zeroadd_x;
            plate.y-=zeroadd_y;
            plate.height += zeroadd_h;
            plate.width += zeroadd_w;
                cv::Mat plateImage = util::cropFromImage(InputImage2,plate);
            PlateInfo plateInfo(plateImage,plate);
            plateInfos.push_back(plateInfo);

        }
    }
}//namespace pr
